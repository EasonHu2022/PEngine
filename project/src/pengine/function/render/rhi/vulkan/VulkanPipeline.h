#pragma once
#include "function/render/rhi/Pipeline.h"
#include "function/render/rhi/Defs.h"
#include "VKHelper.h"
#include <functional>
#include <memory>
namespace pengine
{
	class VulkanPipeline : public Pipeline
	{
	public:
		constexpr static uint32_t MAX_DESCRIPTOR_SET = 1500;
		VulkanPipeline(const PipelineInfo& info);
		virtual ~VulkanPipeline();
		NO_COPYABLE(VulkanPipeline);

		auto init(const PipelineInfo& info) -> bool;
		auto getWidth()->uint32_t override;
		auto getHeight()->uint32_t override;

		auto bind(CommandBuffer* commandBuffer, uint32_t layer = 0, int32_t cubeFace = -1, int32_t mipMapLevel = 0)->FrameBuffer* override;
		auto end(CommandBuffer* commandBuffer) -> void override;
		auto clearRenderTargets(CommandBuffer* commandBuffer) -> void override;

		inline auto getShader() const -> std::shared_ptr<Shader> override
		{
			return shader;
		};

		inline auto getPipelineLayout() const
		{
			return pipelineLayout;
		}

	private:
		auto transitionAttachments() -> void;
		auto createFrameBuffers() -> void;

		std::shared_ptr<Shader>                   shader;
		std::shared_ptr<RenderPass>               renderPass;
		std::vector<std::shared_ptr<FrameBuffer>> framebuffers;

		VkPipelineLayout pipelineLayout;
		VkPipeline       pipeline;
		bool             depthBiasEnabled;
		float            depthBiasConstant;
		float            depthBiasSlope;

	};

};