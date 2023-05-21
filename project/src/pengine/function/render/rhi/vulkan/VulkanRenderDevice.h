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
		auto clearRenderTarget(const std::shared_ptr<Texture>& texture, CommandBuffer* commandBuffer, 
			const glm::vec4& clearColor) -> void override;
		auto clearRenderTarget(const glm::vec4& clearColor = { 0.3f, 0.3f, 0.3f, 1.0f }) -> void override;
	private:
		VkDescriptorPool descriptorPool;
		VkDescriptorSet  descriptorSetPool[16] = {};
	};

	
};