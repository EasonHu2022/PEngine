#pragma once
#include "VulkanBuffer.h"
#include "function/render/rhi/IndexBuffer.h"
namespace pengine
{
	class CommandBuffer;
	class VulkanIndexBuffer : public IndexBuffer, public VulkanBuffer
	{
	public:
		VulkanIndexBuffer(const uint16_t* data, uint32_t count, BufferUsage bufferUsage);
		VulkanIndexBuffer(const uint32_t* data, uint32_t count, BufferUsage bufferUsage);
		~VulkanIndexBuffer();
		NO_COPYABLE(VulkanIndexBuffer);
		auto bind(CommandBuffer* commandBuffer) const -> void override;
		auto unbind() const -> void override;
		auto setData(uint32_t size, const void* data) -> void;
		auto releasePointer() -> void override;
		auto getPointerInternal() -> void* override;
		inline auto getSize() const -> uint32_t override
		{
			return size;
		}

		inline auto setCount(uint32_t indexCount) -> void
		{
			count = indexCount;
		}
		inline auto getCount() const -> uint32_t override
		{
			return count;
		}

	private:
		uint32_t    count = 0;
		uint32_t    size = 0;
		BufferUsage usage;
		bool        mappedBuffer = false;

	};

};