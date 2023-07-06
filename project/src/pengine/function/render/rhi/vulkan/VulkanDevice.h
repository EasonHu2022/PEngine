#pragma once
#include "core/core.h"
#include <memory>
#include <unordered_set>
#include <vector>
#include "VKHelper.h"
#include "VulkanCommmandPool.h"
namespace pengine
{
	class VulkanDevice;
	class VulkanPhysicalDevice
	{
	public:
		VulkanPhysicalDevice();
		NO_COPYABLE(VulkanPhysicalDevice);
		inline auto isExtensionSupported(const std::string& extensionName) const
		{
			return supportedExtensions.find(extensionName) != supportedExtensions.end();
		}

		uint32_t getMemoryTypeIndex(uint32_t typeBits, VkMemoryPropertyFlags properties) const;
		inline operator auto() const
		{
			return physicalDevice;
		}

		inline auto& getProperties() const
		{
			return physicalDeviceProperties;
		};

		inline auto& getQueueFamilyIndices() const
		{
			return indices;
		}
		inline auto& getMemoryProperties() const
		{
			return memoryProperties;
		}
	private:
		std::vector<VkQueueFamilyProperties> queueFamilyProperties;
		std::unordered_set<std::string>      supportedExtensions;
		std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;

		VkPhysicalDevice                 physicalDevice;
		VkPhysicalDeviceProperties       physicalDeviceProperties;
		VkPhysicalDeviceMemoryProperties memoryProperties;

		friend class VulkanDevice;
		QueueFamilyIndices indices;
	};

	class VulkanDevice final
	{
	public:
		VulkanDevice();
		~VulkanDevice();
		bool init();
		void release();
		void createPipelineCache();
		static auto get() -> std::shared_ptr<VulkanDevice>
		{
			if (instance == nullptr)
			{
				instance = std::make_shared<VulkanDevice>();
			}
			return instance;
		}

		inline const auto getPhysicalDevice() const
		{
			return physicalDevice;
		}
		inline operator auto() const
		{
			return device;
		}
		inline auto getAllocator() const
		{
			return allocator;
		}
		inline auto getCommandPool()
		{
			return commandPool;
		}
		inline auto getPipelineCache() const
		{
			return pipelineCache;
		}
		inline auto getGraphicsQueue()
		{
			return graphicsQueue;
		}
		inline auto getPresentQueue()
		{
			return presentQueue;
		}
	private:
		
		static std::shared_ptr<VulkanDevice>  instance;
		std::shared_ptr<VulkanPhysicalDevice> physicalDevice;
		std::shared_ptr<VulkanCommandPool>    commandPool;
		VkDevice device = VK_NULL_HANDLE;
		VkQueue  graphicsQueue = VK_NULL_HANDLE;
		VkQueue  presentQueue = VK_NULL_HANDLE;
		VmaAllocator allocator{};
		VkPipelineCache          pipelineCache = VK_NULL_HANDLE;
		VkDescriptorPool         descriptorPool = VK_NULL_HANDLE;
		VkPhysicalDeviceFeatures enabledFeatures{};
		bool enableDebugMarkers = false;
	};
}