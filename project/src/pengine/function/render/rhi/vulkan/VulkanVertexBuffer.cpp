#include "VulkanVertexBuffer.h"
#include "VulkanCommandBuffer.h"
namespace pengine
{
	VulkanVertexBuffer::VulkanVertexBuffer(const BufferUsage& usage) :
		bufferUsage(usage)
	{
		VulkanBuffer::setUsage(VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
	}

	VulkanVertexBuffer::~VulkanVertexBuffer()
	{
		releasePointer();
	}

	auto VulkanVertexBuffer::resize(uint32_t size) -> void
	{
		if (this->size != size)
		{
			VulkanBuffer::resize(size, nullptr);
		}
	}

	auto VulkanVertexBuffer::setData(uint32_t size, const void* data) -> void
	{
		if (size != this->size)
		{
			VulkanBuffer::resize(size, data);
		}
		else
		{
			VulkanBuffer::setVkData(size, data);
		}
	}

	auto VulkanVertexBuffer::setDataSub(uint32_t size, const void* data, uint32_t offset) -> void
	{
		if (size != this->size)
		{
			VulkanBuffer::resize(size, data);
		}
		else
		{
			VulkanBuffer::setVkData(size, data);
		}
	}

	auto VulkanVertexBuffer::releasePointer() -> void
	{
		if (mappedBuffer)
		{
			VulkanBuffer::flush(size);
			VulkanBuffer::unmap();
			mappedBuffer = false;
		}
	}

	auto VulkanVertexBuffer::bind(CommandBuffer* commandBuffer, Pipeline* pipeline) -> void
	{
		VkDeviceSize offsets[1] = { 0 };
		if (commandBuffer)
			vkCmdBindVertexBuffers(static_cast<VulkanCommandBuffer*>(commandBuffer)->getCommandBuffer(), 0, 1, &buffer, offsets);
	}

	auto VulkanVertexBuffer::unbind() -> void
	{
	}

	auto VulkanVertexBuffer::getPointerInternal() -> void*
	{
		if (!mappedBuffer)
		{
			VulkanBuffer::map();
			mappedBuffer = true;
		}
		return mapped;
	}
};