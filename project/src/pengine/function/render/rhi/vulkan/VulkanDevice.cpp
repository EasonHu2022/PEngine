#include "VulkanDevice.h"
#include "VulkanContext.h"
#include "Application.h"
namespace pengine
{
	namespace
	{
		inline auto getDeviceTypeName(VkPhysicalDeviceType type) -> const std::string
		{
			switch (type)
			{
			case VK_PHYSICAL_DEVICE_TYPE_OTHER:
				return "Other";
			case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
				return "Integrated GPU";
			case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
				return "Discrete GPU";
			case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
				return "Virtual GPU";
			case VK_PHYSICAL_DEVICE_TYPE_CPU:
				return "CPU";
			default:
				return "UNKNOWN";
			}
		}

		inline QueueFamilyIndices lookupQueueFamilyIndices(int32_t flags, std::vector<VkQueueFamilyProperties>& queueFamilyProperties) 
		{
			QueueFamilyIndices indices;

			if (flags & VK_QUEUE_COMPUTE_BIT)
			{
				for (uint32_t i = 0; i < queueFamilyProperties.size(); i++)
				{
					auto& queueFamilyProperty = queueFamilyProperties[i];
					if ((queueFamilyProperty.queueFlags & VK_QUEUE_COMPUTE_BIT) && ((queueFamilyProperty.queueFlags & VK_QUEUE_GRAPHICS_BIT) == 0))
					{
						indices.computeFamily = i;
						break;
					}
				}
			}

			if (flags & VK_QUEUE_TRANSFER_BIT)
			{
				for (uint32_t i = 0; i < queueFamilyProperties.size(); i++)
				{
					auto& queueFamilyProperty = queueFamilyProperties[i];
					if ((queueFamilyProperty.queueFlags & VK_QUEUE_TRANSFER_BIT) && ((queueFamilyProperty.queueFlags & VK_QUEUE_GRAPHICS_BIT) == 0) && ((queueFamilyProperty.queueFlags & VK_QUEUE_COMPUTE_BIT) == 0))
					{
						indices.presentFamily = i;
						break;
					}
				}
			}

			for (uint32_t i = 0; i < queueFamilyProperties.size(); i++)
			{
				if ((flags & VK_QUEUE_TRANSFER_BIT) && !indices.presentFamily.has_value())
				{
					if (queueFamilyProperties[i].queueFlags & VK_QUEUE_TRANSFER_BIT)
						indices.presentFamily = i;
				}

				if ((flags & VK_QUEUE_COMPUTE_BIT) && !indices.computeFamily.has_value())
				{
					if (queueFamilyProperties[i].queueFlags & VK_QUEUE_COMPUTE_BIT)
						indices.computeFamily = i;
				}

				if (flags & VK_QUEUE_GRAPHICS_BIT)
				{
					if (queueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
						indices.graphicsFamily = i;
				}
			}

			return indices;
		}
	}
	VulkanPhysicalDevice::VulkanPhysicalDevice()
	{
		uint32_t numGPUS = 0;
		auto vkContext = std::static_pointer_cast<VulkanContext>(Application::getGraphicsContext());
		auto vkInstance = vkContext->getVkInstance();
		vkEnumeratePhysicalDevices(vkInstance, &numGPUS, VK_NULL_HANDLE);
		if (numGPUS == 0)
		{
			PLOGE("No GPU Found!");
		}
		std::vector<VkPhysicalDevice> physicalDevices(numGPUS);
		vkEnumeratePhysicalDevices(vkInstance, &numGPUS, physicalDevices.data());
		for (VkPhysicalDevice device : physicalDevices)
		{
			vkGetPhysicalDeviceProperties(device, &physicalDeviceProperties);
			if (physicalDeviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
			{
				physicalDevice = device;
				break;
			}
		}
		vkGetPhysicalDeviceProperties(physicalDevice, &physicalDeviceProperties);
		vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memoryProperties);
		PLOGI("Vulkan : {0}.{1}.{2}", VK_VERSION_MAJOR(physicalDeviceProperties.apiVersion), VK_VERSION_MINOR(physicalDeviceProperties.apiVersion), VK_VERSION_PATCH(physicalDeviceProperties.apiVersion));
		PLOGI("GPU : {0}", std::string(physicalDeviceProperties.deviceName));
		PLOGI("Vendor ID : {0}", physicalDeviceProperties.vendorID);
		PLOGI("Device Type : {0}", getDeviceTypeName(physicalDeviceProperties.deviceType));
		PLOGI("Driver Version : {0}.{1}.{2}", VK_VERSION_MAJOR(physicalDeviceProperties.driverVersion), VK_VERSION_MINOR(physicalDeviceProperties.driverVersion), VK_VERSION_PATCH(physicalDeviceProperties.driverVersion));
		uint32_t queueFamilyCount;
		vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);
		PENGINE_ASSERT(queueFamilyCount > 0, "");
		queueFamilyProperties.resize(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice,&queueFamilyCount,queueFamilyProperties.data());

		uint32_t extCount = 0;
		vkEnumerateDeviceExtensionProperties(physicalDevice,nullptr,&extCount,nullptr);
		if (extCount > 0)
		{
			std::vector<VkExtensionProperties> extensions(extCount);
			if (vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extCount, &extensions.front()) == VK_SUCCESS)
			{
				PLOGI("Selected physical device has {0} extensions", extensions.size());
				for (const auto& ext : extensions)
				{
					supportedExtensions.emplace(ext.extensionName);
				}
			}
		}
		static const float defaultQueuePriority(0.0f);
		int32_t requestedQueueTypes = VK_QUEUE_GRAPHICS_BIT;
		indices = lookupQueueFamilyIndices(requestedQueueTypes, queueFamilyProperties);
		// Graphics queue
		if (requestedQueueTypes & VK_QUEUE_GRAPHICS_BIT)
		{
			VkDeviceQueueCreateInfo queueInfo{};
			queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queueInfo.queueFamilyIndex = indices.graphicsFamily.value();
			queueInfo.queueCount = 1;
			queueInfo.pQueuePriorities = &defaultQueuePriority;
			queueCreateInfos.emplace_back(queueInfo);
		}

		// compute queue
		if (requestedQueueTypes & VK_QUEUE_COMPUTE_BIT)
		{
			if (indices.computeFamily != indices.graphicsFamily)
			{
				// If compute family index differs, we need an additional queue create info for the compute queue
				VkDeviceQueueCreateInfo queueInfo{};
				queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
				queueInfo.queueFamilyIndex = indices.computeFamily.value();
				queueInfo.queueCount = 1;
				queueInfo.pQueuePriorities = &defaultQueuePriority;
				queueCreateInfos.emplace_back(queueInfo);
			}
		}

		// transfer queue
		if (requestedQueueTypes & VK_QUEUE_TRANSFER_BIT)
		{
			if ((indices.presentFamily != indices.graphicsFamily) && (indices.presentFamily != indices.computeFamily))
			{
				// If compute family index differs, we need an additional queue create info for the compute queue
				VkDeviceQueueCreateInfo queueInfo{};
				queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
				queueInfo.queueFamilyIndex = indices.presentFamily.value();
				queueInfo.queueCount = 1;
				queueInfo.pQueuePriorities = &defaultQueuePriority;
				queueCreateInfos.emplace_back(queueInfo);
			}
		}
	}

	uint32_t VulkanPhysicalDevice::getMemoryTypeIndex(uint32_t typeBits, VkMemoryPropertyFlags properties) const 
	{
		for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; i++)
		{
			if ((typeBits & 1) == 1)
			{
				if ((memoryProperties.memoryTypes[i].propertyFlags & properties) == properties)
					return i;
			}
			typeBits >>= 1;
		}
		PENGINE_ASSERT(false, "Could not find a suitable memory type!");
		return -1;
	}
	VulkanDevice::VulkanDevice()
	{
	}

	VulkanDevice::~VulkanDevice()
	{
		vkDestroyPipelineCache(device, pipelineCache, VK_NULL_HANDLE);
		vmaDestroyAllocator(allocator);
		if (device != nullptr)
			vkDestroyDevice(device, nullptr);
	}

	bool VulkanDevice::init()
	{
		physicalDevice = std::make_shared<VulkanPhysicalDevice>();
		VkPhysicalDeviceFeatures physicalDeviceFeatures;
		vkGetPhysicalDeviceFeatures(*physicalDevice, &physicalDeviceFeatures);
		std::vector<const char*> deviceExtensions = {
			VK_KHR_SWAPCHAIN_EXTENSION_NAME,
			VK_KHR_SHADER_NON_SEMANTIC_INFO_EXTENSION_NAME
		};
		if (physicalDevice->isExtensionSupported(VK_EXT_DEBUG_UTILS_EXTENSION_NAME))
		{
			deviceExtensions.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
			enableDebugMarkers = true;
		}
		VkPhysicalDeviceDescriptorIndexingFeaturesEXT indexingFeatures{};
		indexingFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES_EXT;
		indexingFeatures.runtimeDescriptorArray = VK_TRUE;
		indexingFeatures.descriptorBindingPartiallyBound = VK_TRUE;

#if defined(PLATFORM_MACOS) || defined(PLATFORM_IOS)
		// https://vulkan.lunarg.com/doc/view/1.2.162.0/mac/1.2-extensions/vkspec.html#VUID-VkDeviceCreateInfo-pProperties-04451
		if (physicalDevice->isExtensionSupported("VK_KHR_portability_subset"))
		{
			deviceExtensions.emplace_back("VK_KHR_portability_subset");
		}
#endif
		if (physicalDevice->isExtensionSupported(VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME))
		{
			deviceExtensions.emplace_back(VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME);
		}
		VkDeviceCreateInfo deviceCreateInfo{};
		deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		deviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(physicalDevice->queueCreateInfos.size());
		deviceCreateInfo.pQueueCreateInfos = physicalDevice->queueCreateInfos.data();
		deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
		deviceCreateInfo.ppEnabledExtensionNames = deviceExtensions.data();
		deviceCreateInfo.pEnabledFeatures = &physicalDeviceFeatures;
		deviceCreateInfo.enabledLayerCount = 0;
		deviceCreateInfo.pNext = (void*)&indexingFeatures;
		auto result = vkCreateDevice(*physicalDevice, &deviceCreateInfo, VK_NULL_HANDLE, &device);
		if (result != VK_SUCCESS)
		{
			PLOGE("[VULKAN] vkCreateDevice() failed!");
			return false;
		}
		volkLoadDevice(device);
		vkGetDeviceQueue(device, physicalDevice->indices.graphicsFamily.value(), 0, &graphicsQueue);
		vkGetDeviceQueue(device, physicalDevice->indices.graphicsFamily.value(), 0, &presentQueue);
	{


			//load vma funcs
		VmaAllocatorCreateInfo allocatorInfo = {};
		allocatorInfo.physicalDevice = *physicalDevice;
		allocatorInfo.device = device;
		allocatorInfo.instance = VulkanContext::get()->getVkInstance();

		VmaVulkanFunctions fn;
		fn.vkAllocateMemory = vkAllocateMemory;
		fn.vkBindBufferMemory = vkBindBufferMemory;
		fn.vkBindImageMemory = vkBindImageMemory;
		fn.vkCmdCopyBuffer = vkCmdCopyBuffer;
		fn.vkCreateBuffer = vkCreateBuffer;
		fn.vkCreateImage = vkCreateImage;
		fn.vkDestroyBuffer = vkDestroyBuffer;
		fn.vkDestroyImage = vkDestroyImage;
		fn.vkFlushMappedMemoryRanges = vkFlushMappedMemoryRanges;
		fn.vkFreeMemory = vkFreeMemory;
		fn.vkGetBufferMemoryRequirements = vkGetBufferMemoryRequirements;
		fn.vkGetImageMemoryRequirements = vkGetImageMemoryRequirements;
		fn.vkGetPhysicalDeviceMemoryProperties = vkGetPhysicalDeviceMemoryProperties;
		fn.vkGetPhysicalDeviceProperties = vkGetPhysicalDeviceProperties;
		fn.vkInvalidateMappedMemoryRanges = vkInvalidateMappedMemoryRanges;
		fn.vkMapMemory = vkMapMemory;
		fn.vkUnmapMemory = vkUnmapMemory;
		fn.vkGetBufferMemoryRequirements2KHR = vkGetBufferMemoryRequirements2KHR;
		fn.vkGetImageMemoryRequirements2KHR = vkGetImageMemoryRequirements2KHR;
		fn.vkBindImageMemory2KHR = 0;
		fn.vkBindBufferMemory2KHR = 0;
		fn.vkGetPhysicalDeviceMemoryProperties2KHR = 0;
		fn.vkGetImageMemoryRequirements2KHR = 0;
		fn.vkGetBufferMemoryRequirements2KHR = 0;
		allocatorInfo.pVulkanFunctions = &fn;

		if (vmaCreateAllocator(&allocatorInfo, &allocator) != VK_SUCCESS)
		{
			PLOGE("[VULKAN] Failed to create VMA allocator");
		}
	}	
		
		commandPool = std::make_shared<VulkanCommandPool>(physicalDevice->indices.graphicsFamily.value(), VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
		createPipelineCache();
		return true;
	}
	void VulkanDevice::createPipelineCache()
	{
		VkPipelineCacheCreateInfo pipelineCacheCI{};
		pipelineCacheCI.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
		pipelineCacheCI.pNext = NULL;
		vkCreatePipelineCache(device, &pipelineCacheCI, VK_NULL_HANDLE, &pipelineCache);
	}

	std::shared_ptr<VulkanDevice> VulkanDevice::instance;
};

