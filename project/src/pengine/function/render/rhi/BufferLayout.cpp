#include "BufferLayout.h"
#include <string>
namespace pengine
{
	BufferLayout::BufferLayout()
	{
	}

	void BufferLayout::reset()
	{
		size = 0;
		layouts.clear();
	}
	void BufferLayout::computeStride()
	{
		uint32_t stride = 0;
		for (auto& layout : layouts)
		{
			auto size = layout.offset;
			layout.offset = stride;
			stride += size;
		}
		size = stride;
	}
	void BufferLayout::push(const std::string& name, Format format, uint32_t size, uint32_t location, bool normalized)
	{
		layouts[location] = { name, format, size, normalized };
	}

	template <>
	void BufferLayout::push<uint32_t>(const std::string& name, uint32_t location, bool normalized) 
	{
		push(name, Format::R32_UINT, sizeof(uint32_t), location, normalized);
	}

	template <>
	void BufferLayout::push<uint8_t>(const std::string& name, uint32_t location, bool normalized) 
	{
		push(name, Format::R8_UINT, sizeof(uint8_t), location, normalized);
	}

	template <>
	void BufferLayout::push<float>(const std::string& name, uint32_t location, bool normalized) 
	{
		push(name, Format::R32_FLOAT, sizeof(float), location, normalized);
	}
};