#include "VulkanIndexBuffer.h"
#include "VulkanCommandBuffer.h"
namespace pengine
{
	VulkanIndexBuffer::VulkanIndexBuffer(const uint16_t* data, uint32_t _count, BufferUsage bufferUsage) :
		VulkanBuffer(VK_BUFFER_USAGE_INDEX_BUFFER_BIT, _count * sizeof(uint16_t),data),size(_count * sizeof(uint16_t)),count(_count),
		usage(bufferUsage)
	{
	}
	VulkanIndexBuffer::VulkanIndexBuffer(const uint32_t* data, uint32_t _count, BufferUsage bufferUsage) :
		VulkanBuffer(VK_BUFFER_USAGE_INDEX_BUFFER_BIT, _count * sizeof(uint32_t), data), size(_count * sizeof(uint32_t)), count(_count),
		usage(bufferUsage)
	{
	}
	VulkanIndexBuffer::~VulkanIndexBuffer()
	{
		if (mappedBuffer)
		{
			VulkanBuffer::flush(size);
			VulkanBuffer::unmap();
			mappedBuffer = false;
		}
	}
	auto VulkanIndexBuffer::bind(CommandBuffer* commandBuffer) const -> void 
	{
		vkCmdBindIndexBuffer(static_cast<VulkanCommandBuffer*>(commandBuffer)->getCommandBuffer(), buffer, 0, VK_INDEX_TYPE_UINT32);
	}
	auto VulkanIndexBuffer::unbind() const -> void 
	{
		
	}
	auto VulkanIndexBuffer::setData(uint32_t size, const void* data) -> void
	{
		setVkData(size, data);
	}
	auto VulkanIndexBuffer::releasePointer() -> void 
	{
		if (mappedBuffer)
		{
			VulkanBuffer::flush(size);
			VulkanBuffer::unmap();
			mappedBuffer = false;
		}
	}
	auto VulkanIndexBuffer::getPointerInternal() -> void* 
	{
		if (!mappedBuffer)
		{
			VulkanBuffer::map();
			mappedBuffer = true;
		}
		return mapped;
	}
};