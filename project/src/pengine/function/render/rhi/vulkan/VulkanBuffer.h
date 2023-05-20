#pragma once
#include "VKHelper.h"
#include "core/core.h"
namespace pengine
{
	class VulkanBuffer
	{
	public:
		VulkanBuffer();
		VulkanBuffer(VkBufferUsageFlags usage, uint32_t size, const void* data);
		virtual ~VulkanBuffer();
		NO_COPYABLE(VulkanBuffer);

		void resize(uint32_t size, const void* data);
		void init(VkBufferUsageFlags usage, uint32_t size, const void* data);
		void map(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
		void unmap();
		void flush(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
		void invalidate(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
		void setVkData(uint32_t size, const void* data, uint32_t offset = 0);

		inline auto setUsage(VkBufferUsageFlags flags)
		{
			usage = flags;
		}
		inline auto& getVkBuffer()
		{
			return buffer;
		}
		inline const auto& getVkBuffer() const
		{
			return buffer;
		}
		inline const auto& getBufferInfo() const
		{
			return desciptorBufferInfo;
		}

	protected:
		void release();

		VkDescriptorBufferInfo desciptorBufferInfo{};
		VkBuffer               buffer = VK_NULL_HANDLE;
		VkDeviceMemory         memory = VK_NULL_HANDLE;
		VkDeviceSize           size = 0;
		VkDeviceSize           alignment = 0;
		void* mapped = nullptr;
		VkBufferUsageFlags     usage;
		VmaAllocation allocation{};
		VmaAllocation mappedAllocation{};
	};
};