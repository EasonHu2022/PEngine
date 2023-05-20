#include "VulkanBuffer.h"
#include "VulkanDevice.h"
#include "VulkanContext.h"
#include "VulkanSwapChain.h"
namespace pengine
{
	VulkanBuffer::VulkanBuffer()
	{
	}
	VulkanBuffer::VulkanBuffer(VkBufferUsageFlags usage, uint32_t size, const void* data)
		: usage(usage), size(size)
	{
		init(usage, size, data);
	}
	VulkanBuffer::~VulkanBuffer()
	{
		release();
	}
	void VulkanBuffer::resize(uint32_t size, const void* data)
	{
		release();
		init(usage, size, data);
	}
	void VulkanBuffer::init(VkBufferUsageFlags usage, uint32_t size, const void* data)
	{
		//param for creating
		this->usage = usage;
		this->size = size;
		VkBufferCreateInfo bufferInfo = {};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = size;
		bufferInfo.usage = usage;
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		VmaAllocationCreateInfo vmaAllocInfo = {};
		vmaAllocInfo.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;
		vmaCreateBuffer(VulkanDevice::get()->getAllocator(), &bufferInfo, &vmaAllocInfo, &buffer, &allocation, nullptr);
		if (data != nullptr)
			setVkData(size, data);
	}
	void VulkanBuffer::map(VkDeviceSize size, VkDeviceSize offset)
	{
		VK_CHECK_RESULT(vmaMapMemory(VulkanDevice::get()->getAllocator(), allocation, &mapped));
	}
	void VulkanBuffer::unmap()
	{
		if (mapped)
		{
			vmaUnmapMemory(VulkanDevice::get()->getAllocator(), allocation);
			mapped = nullptr;
		}
	}
	void VulkanBuffer::flush(VkDeviceSize size, VkDeviceSize offset)
	{
		vmaFlushAllocation(VulkanDevice::get()->getAllocator(), allocation, offset, size);
	}
	void VulkanBuffer::invalidate(VkDeviceSize size, VkDeviceSize offset)
	{
		vmaInvalidateAllocation(VulkanDevice::get()->getAllocator(), allocation, offset, size);
	}
	void VulkanBuffer::setVkData(uint32_t size, const void* data, uint32_t offset)
	{
		map(size, offset);
		memcpy(reinterpret_cast<uint8_t*>(mapped) + offset, data, size);
		unmap();
	}
	void VulkanBuffer::release()
	{
		if (buffer)
		{
			auto& queue = VulkanContext::getDeletionQueue();
			auto  buffer = this->buffer;
			auto  bufferId = VulkanContext::get()->getSwapChain()->getCurrentBufferIndex();
			auto alloc = allocation;
			queue.emplace([buffer, alloc, bufferId] {
				PLOGI("bufferId : {0}", bufferId);
				vmaDestroyBuffer(VulkanDevice::get()->getAllocator(), buffer, alloc); });
		}
	}
};