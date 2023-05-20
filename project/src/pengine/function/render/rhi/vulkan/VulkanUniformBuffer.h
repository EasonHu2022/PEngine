#pragma once
#include "function/render/rhi/UniformBuffer.h"
#include "VulkanBuffer.h"
namespace pengine
{
	class VulkanUniformBuffer : public VulkanBuffer, public UniformBuffer
	{
	public:
		VulkanUniformBuffer(uint32_t size, const void* data);
		VulkanUniformBuffer();
		~VulkanUniformBuffer();

		void init(uint32_t size, const void* data)  override;
		void setDynamicData(uint32_t size, uint32_t typeSize, const void* data)  override;
		void setData(uint32_t size, const void* data)  override;
		inline auto setData(const void* data) -> void override
		{
			setData((uint32_t)size, data);
		};

		inline auto getBuffer() const -> uint8_t* override
		{
			return nullptr;
		}

		inline auto getMemory()
		{
			return &memory;
		}

	};

};