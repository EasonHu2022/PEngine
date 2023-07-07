#include "VulkanContext.h"
#include "Application.h"
#include "VulkanDevice.h"
#include "function/render/rhi/SwapChain.h"
#include "function/render/rhi/vulkan/VulkanCommandBuffer.h"
#include "VulkanRenderDevice.h"

#define VK_LAYER_LUNARG_STANDARD_VALIDATION_NAME "VK_LAYER_LUNARG_standard_validation"
#define VK_LAYER_LUNARG_ASSISTENT_LAYER_NAME "VK_LAYER_LUNARG_assistant_layer"
#define VK_LAYER_LUNARG_VALIDATION_NAME "VK_LAYER_KHRONOS_validation"
#define VK_LAYER_RENDERDOC_CAPTURE_NAME "VK_LAYER_RENDERDOC_Capture"

namespace pengine
{
	namespace
	{
		VkDebugReportCallbackEXT reportCallback = {};
		inline auto getRequiredLayers()
		{
			std::vector<const char*> layers;
			if constexpr (VkConfig::StandardValidationLayer)
				layers.emplace_back(VK_LAYER_LUNARG_STANDARD_VALIDATION_NAME);

			if constexpr (VkConfig::AssistanceLayer)
				layers.emplace_back(VK_LAYER_LUNARG_ASSISTENT_LAYER_NAME);

			if constexpr (VkConfig::EnableValidationLayers)
			{
				layers.emplace_back(VK_LAYER_LUNARG_VALIDATION_NAME);
			}

			//	layers.emplace_back(VK_LAYER_RENDERDOC_CAPTURE_NAME);
			return layers;
		}

		inline auto getRequiredExtensions()
		{
			std::vector<const char*> extensions;

			if constexpr (VkConfig::EnableValidationLayers)
			{
				extensions.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
			}

			extensions.emplace_back("VK_EXT_debug_report");
			extensions.emplace_back("VK_KHR_surface");
			extensions.emplace_back("VK_KHR_win32_surface");
			return extensions;
		}

		inline auto checkValidationLayerSupport(std::vector<VkLayerProperties>& layers, std::vector<const char*>& validationLayers)
		{
			uint32_t layerCount = 0;
			vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

			layers.resize(layerCount);
			vkEnumerateInstanceLayerProperties(&layerCount, layers.data());

			for (const char* layerName : validationLayers)
			{
				bool layerFound = false;
				for (const auto& layerProperties : layers)
				{
					if (strcmp(layerName, layerProperties.layerName) == 0)
					{
						layerFound = true;
						break;
					}
				}
				if (!layerFound)
				{
					return false;
				}
			}
			return true;
		}
		inline auto checkExtensionSupport(std::vector<VkExtensionProperties>& properties, std::vector<const char*>& extensions)
		{
			uint32_t extensionCount = 0;
			vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

			properties.resize(extensionCount);
			vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, properties.data());

			bool extensionSupported = true;

			for (int i = 0; i < extensions.size(); i++)
			{
				const char* extensionName = extensions[i];
				bool        layerFound = false;

				for (const auto& layerProperties : properties)
				{
					if (strcmp(extensionName, layerProperties.extensionName) == 0)
					{
						layerFound = true;
						break;
					}
				}

				if (!layerFound)
				{
					extensions.erase(extensions.begin() + i);
					extensionSupported = false;
					PLOGE("Extension not supported {0}", extensionName);
				}
			}
			return extensionSupported;
		}
		
		inline auto debugCallback(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objType, uint64_t sourceObj, size_t location, int32_t msgCode, const char* pLayerPrefix, const char* pMsg, void* userData) -> VkBool32
		{
			if (!flags)
				return VK_FALSE;

			if (flags & VK_DEBUG_REPORT_ERROR_BIT_EXT)
			{
				PLOGW("[VULKAN] - ERROR : [{0}] Code {1}  : {2}", pLayerPrefix, msgCode, pMsg);
				int32_t i = 0;
			};
			if (flags & VK_DEBUG_REPORT_WARNING_BIT_EXT)
			{
				PLOGW("[VULKAN] - WARNING : [{0}] Code {1}  : {2}", pLayerPrefix, msgCode, pMsg);
			};
			if (flags & VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT)
			{
				PLOGW("[VULKAN] - PERFORMANCE : [{0}] Code {1}  : {2}", pLayerPrefix, msgCode, pMsg);
			};
			if (flags & VK_DEBUG_REPORT_INFORMATION_BIT_EXT)
			{
				PLOGW("[VULKAN] - INFO : [{0}] Code {1}  : {2}", pLayerPrefix, msgCode, pMsg);
			}
			if (flags & VK_DEBUG_REPORT_DEBUG_BIT_EXT)
			{
				PLOGI("[VULKAN] - DEBUG : [{0}] Code {1}  : {2}", pLayerPrefix, msgCode, pMsg);
			}
			return VK_FALSE;
		}

		inline auto createDebugReportCallbackEXT(VkInstance instance, const VkDebugReportCallbackCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugReportCallbackEXT* pCallback) -> VkResult
		{
			auto func = reinterpret_cast<PFN_vkCreateDebugReportCallbackEXT>(vkGetInstanceProcAddr(instance, "vkCreateDebugReportCallbackEXT"));
			if (func != nullptr)
			{
				return func(instance, pCreateInfo, pAllocator, pCallback);
			}
			else
			{
				return VK_ERROR_EXTENSION_NOT_PRESENT;
			}
		}

	
	};

	VulkanContext::VulkanContext()
	{
		////load vulkan
		if (auto const res = volkInitialize(); VK_SUCCESS != res)
		{
			PLOGE("Error: unable to load Vulkan API\n");

			return;
		}
	}
	VulkanContext::~VulkanContext()
	{
		
	}

	auto VulkanContext::setupDebug() -> void
	{
		
		if constexpr (VkConfig::EnableValidationLayers)
		{
			VkDebugReportCallbackCreateInfoEXT createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
			createInfo.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT | VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT | VK_DEBUG_REPORT_INFORMATION_BIT_EXT;
			createInfo.pfnCallback = reinterpret_cast<PFN_vkDebugReportCallbackEXT>(debugCallback);
			if (createDebugReportCallbackEXT(vkInstance, &createInfo, nullptr, &reportCallback) != VK_SUCCESS)
			{
				PLOGC("[VULKAN] Failed to set up debug callback!");
			}
		}
	}
	void VulkanContext::init()
	{
		
		createInstance();
		setupDebug();
		VulkanDevice::get()->init();
		

		auto& window = Application::getWindow();
		swapChain = SwapChain::create(window->getWidth(), window->getHeight());
		swapChain->init(false, window.get());

		//create tracy ctx
		//if tracy
		VkPhysicalDevice pd = *(VulkanDevice::get()->getPhysicalDevice().get());
		VkDevice d = *(VulkanDevice::get().get());
		VkCommandBuffer cb = ((VulkanCommandBuffer*)swapChain->getCurrentCommandBuffer())->getCommandBuffer();
		tracyCtx = TracyVkContext(pd, d, VulkanDevice::get()->getGraphicsQueue(), cb);
	}
	void VulkanContext::release()
	{
		pipelineCache.clear();
		frameBufferCache.clear();
		swapChain->release();
		for (int32_t i = 0; i < 3; i++)
		{
			deletionQueue[i].flush();
		}
		swapChain.reset();
		vkDestroyDescriptorPool(*VulkanDevice::get(), std::static_pointer_cast<VulkanRenderDevice>(Application::getRenderDevice())->getDescriptorPool(), VK_NULL_HANDLE);

		if (reportCallback)
		{
			PFN_vkDestroyDebugReportCallbackEXT destoryCallback = (PFN_vkDestroyDebugReportCallbackEXT)vkGetInstanceProcAddr(vkInstance, "vkDestroyDebugReportCallbackEXT");
			destoryCallback(vkInstance, reportCallback, VK_NULL_HANDLE);
		}
		VulkanDevice::get()->release();
		
		vkDestroyInstance(vkInstance, nullptr);
	}
	void VulkanContext::present()
	{
	}

	auto VulkanContext::getMinUniformBufferOffsetAlignment() const -> size_t
	{
		return 0;
	}

	void VulkanContext::waitIdle() const
	{
		vkDeviceWaitIdle(*VulkanDevice::get());
	}
	void VulkanContext::onImGui()
	{
	}
	auto VulkanContext::get() -> std::shared_ptr<VulkanContext>
	{
		return std::static_pointer_cast<VulkanContext>(Application::getGraphicsContext());
	}
	auto VulkanContext::getDeletionQueue() -> CommandQueue&
	{
		/// <summary>
		/// fix bug , when init , swapchain haven't init the bfindex , so get wrong
		/// </summary>
		/// <returns></returns>
		return get()->deletionQueue[(get()->vkInstance && Application::getWindow()) ? get()->getSwapChain()->getCurrentBufferIndex() : 0];
	}
	auto VulkanContext::getDeletionQueue(uint32_t index) -> CommandQueue&
	{
		PENGINE_ASSERT(index < 3, "Unsupported Frame Index");
		return get()->deletionQueue[index];
	}
	auto VulkanContext::getTracyCtx() -> TracyVkCtx
	{
		return get()->tracyCtx;
	}
	void VulkanContext::createInstance()
	{
		
		instanceLayerNames = getRequiredLayers();
		instanceExtensionNames = getRequiredExtensions();
		if (VkConfig::EnableValidationLayers && !checkValidationLayerSupport(instanceLayers, instanceLayerNames))
		{
			PLOGE("[VULKAN] Validation layers requested, but not available!");
		}

		if (!checkExtensionSupport(instanceExtensions, instanceExtensionNames))
		{
			PLOGE("[VULKAN] Extensions requested are not available!");
		}

		VkApplicationInfo appInfo = {};
		appInfo.pApplicationName = "Editor";
		appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.pEngineName = "PEngine";
		appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.apiVersion = VK_API_VERSION_1_1;

		VkInstanceCreateInfo createInfo = {};
		createInfo.pApplicationInfo = &appInfo;
		createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		createInfo.enabledExtensionCount = static_cast<uint32_t>(instanceExtensionNames.size());
		createInfo.ppEnabledExtensionNames = instanceExtensionNames.data();
		createInfo.enabledLayerCount = static_cast<uint32_t>(instanceLayerNames.size());
		createInfo.ppEnabledLayerNames = instanceLayerNames.data();

		VkValidationFeatureEnableEXT enabled[] = { VK_VALIDATION_FEATURE_ENABLE_DEBUG_PRINTF_EXT };
		VkValidationFeaturesEXT      features{ VK_STRUCTURE_TYPE_VALIDATION_FEATURES_EXT };
		features.disabledValidationFeatureCount = 0;
		features.enabledValidationFeatureCount = 1;
		features.pDisabledValidationFeatures = nullptr;
		features.pEnabledValidationFeatures = enabled;
		features.pNext = createInfo.pNext;

		createInfo.pNext = &features;

		VK_CHECK_RESULT(vkCreateInstance(&createInfo, nullptr, &vkInstance));
		
		//load instance apis
		volkLoadInstance(vkInstance);

	}
};