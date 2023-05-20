#pragma once
#include "function/render/rhi/RenderDevice.h"
#include "VKHelper.h"
namespace pengine
{
	class Texture;
	class VulkanRenderDevice : public RenderDevice
	{
	public:
		VulkanRenderDevice();
		~VulkanRenderDevice();
		void begin() override;
		void init() override;
		void onResize(uint32_t width, uint32_t height) override;
		void drawSplashScreen(const std::shared_ptr<Texture>& texture) override;
		auto presentInternal() -> void ;
		auto presentInternal(CommandBuffer* commandBuffer) -> void ;
		inline auto getDescriptorPool() const
		{
			return descriptorPool;
		}

	private:
		VkDescriptorPool descriptorPool;
		VkDescriptorSet  descriptorSetPool[16] = {};
	};

	
};