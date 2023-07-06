#pragma once
#include "ImGuiRenderer.h"
#include "function/render/rhi/vulkan/VulkanTexture.h"
#include "function/render/rhi/vulkan/VulkanRenderPass.h"
#include <imgui_impl_vulkan.h>
namespace pengine
{
	class VulkanImGuiRenderer : public ImGuiRenderer
	{
	public:
		VulkanImGuiRenderer(uint32_t width, uint32_t height, bool clearScreen);
		~VulkanImGuiRenderer();

		virtual void init() override;

		virtual void newFrame(const float& delta) override;

		virtual void render(CommandBuffer* commandBuffer) override;

		virtual void onResize(uint32_t width, uint32_t height) override;

		virtual void rebuildFontTexture() override;

		virtual ImTextureID addTexture(Texture2D* texture) override;
		virtual void removeTexture(ImTextureID image) override;
		virtual void release() override ;
	private:
		auto setupVulkanWindowData(ImGui_ImplVulkanH_Window* wd, VkSurfaceKHR surface, int32_t width, int32_t height) -> void;

		void* windowHandle = nullptr;
		bool                       clearScreen = false;
		std::shared_ptr<Texture2D> fontTexture;
		uint32_t                   width;
		uint32_t                   height;

		std::vector<std::shared_ptr<FrameBuffer>> frameBuffers;
		std::shared_ptr<RenderPass>               renderPass;
		std::shared_ptr<DescriptorSet>			  descriptorSet;

	};
}