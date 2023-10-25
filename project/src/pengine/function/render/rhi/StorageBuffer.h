#pragma once
#include <stdint.h>
#include <memory>
namespace pengine
{
	class StorageBuffer
	{
	public:
		virtual ~StorageBuffer() = default;
		static auto create()->std::shared_ptr<StorageBuffer>;
		static auto create(uint32_t size, const void* data)->std::shared_ptr<StorageBuffer>;
		virtual auto init(uint32_t size, const void* data) -> void = 0;
		virtual auto setData(const void* data) -> void = 0;
		virtual auto setData(uint32_t size, const void* data) -> void = 0;
		virtual auto setDynamicData(uint32_t size, uint32_t typeSize, const void* data) -> void = 0;
		virtual auto getBuffer() const->uint8_t* = 0;

	};


};