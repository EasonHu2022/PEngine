#pragma once
#include "DescriptorSet.h"
#include <vector>
#include "core/log/PLog.h"
namespace pengine
{
	struct BufferElement
	{
		std::string name;
		Format      format;
		uint32_t    offset = 0;
		bool        normalized = false;
	};

	class BufferLayout
	{
	public:
		BufferLayout();
		void reset();

		template <typename T>
		auto push(const std::string& name, uint32_t level, bool normalized = false) -> void
		{
			PENGINE_ASSERT(false, "Unknown type!");
		}

		inline auto& getLayout() const
		{
			return layouts;
		}

		inline auto& getLayout()
		{
			return layouts;
		}

		inline auto getStride() const
		{
			return size;
		}

		void computeStride();

	private:
		void push(const std::string &name, Format format, uint32_t size, uint32_t location, bool normalized);

		uint32_t                   size = 0;
		std::vector<BufferElement> layouts;
	};
	template<>
	void BufferLayout::push<float>(const std::string& name, uint32_t level, bool normalized);
	template<>
	void BufferLayout::push<uint32_t>(const std::string& name, uint32_t level, bool normalized);
	template<>
	void BufferLayout::push<uint8_t>(const std::string& name, uint32_t level, bool normalized);
};