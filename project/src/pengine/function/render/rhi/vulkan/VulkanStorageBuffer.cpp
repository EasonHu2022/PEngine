#include "VulkanStorageBuffer.h"
namespace pengine
{
	VulkanStorageBuffer::VulkanStorageBuffer(uint32_t size, const void* data)
	{
		init( size, data);
	}
	VulkanStorageBuffer::VulkanStorageBuffer()
	{
	}
	VulkanStorageBuffer::~VulkanStorageBuffer()
	{
	}
	void VulkanStorageBuffer::init(uint32_t size, const void* data)
	{
		VulkanBuffer::init(VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, size, data);
	}
	void VulkanStorageBuffer::setDynamicData(uint32_t size, uint32_t typeSize, const void* data)
	{
		VulkanBuffer::map();
		memcpy(mapped, data, size);
		VulkanBuffer::flush(size);
		VulkanBuffer::unmap();
	}
	void VulkanStorageBuffer::setData(uint32_t size, const void* data)
	{
		VulkanBuffer::map();
		memcpy(mapped, data, size);
		VulkanBuffer::unmap();
	}
};