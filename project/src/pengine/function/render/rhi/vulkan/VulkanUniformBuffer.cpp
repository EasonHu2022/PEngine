#include "VulkanUniformBuffer.h"
namespace pengine
{
	VulkanUniformBuffer::VulkanUniformBuffer(uint32_t size, const void* data)
	{
		init( size, data);
	}
	VulkanUniformBuffer::VulkanUniformBuffer()
	{
	}
	VulkanUniformBuffer::~VulkanUniformBuffer()
	{
	}
	void VulkanUniformBuffer::init(uint32_t size, const void* data)
	{
		VulkanBuffer::init(VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, size, data);
	}
	void VulkanUniformBuffer::setDynamicData(uint32_t size, uint32_t typeSize, const void* data)
	{
		VulkanBuffer::map();
		memcpy(mapped, data, size);
		VulkanBuffer::flush(size);
		VulkanBuffer::unmap();
	}
	void VulkanUniformBuffer::setData(uint32_t size, const void* data)
	{
		VulkanBuffer::map();
		memcpy(mapped, data, size);
		VulkanBuffer::unmap();
	}
};