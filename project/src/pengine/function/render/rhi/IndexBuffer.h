#pragma once
#include "BufferUsage.h"
#include "CommandBuffer.h"
#include "core/core.h"
namespace pengine
{
	class IndexBuffer
	{
	public:
		virtual ~IndexBuffer() = default;
		virtual auto bind(CommandBuffer * commandBuffer = nullptr) const -> void = 0;
		virtual auto unbind() const -> void = 0;
		virtual auto getCount() const->uint32_t = 0;
		virtual auto setCount(uint32_t indexCount) -> void = 0;

		virtual auto releasePointer() -> void {};

		virtual auto getSize() const -> uint32_t
		{
			return 0;
		}

		template <typename T>
		inline auto getPointer()
		{
			return static_cast<T*>(getPointerInternal());
		}

	public:
		static auto create(const uint16_t* data, uint32_t count, BufferUsage bufferUsage = BufferUsage::Static)->std::shared_ptr<IndexBuffer>;
		static auto create(const uint32_t* data, uint32_t count, BufferUsage bufferUsage = BufferUsage::Static)->std::shared_ptr<IndexBuffer>;

	protected:
		virtual auto getPointerInternal() -> void*
		{
			return nullptr;
		}

	};


};