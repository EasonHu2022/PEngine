#pragma once
#include <memory>
#include "Defs.h"
namespace pengine
{

	class Pipeline
	{
	public:
		static auto get(const PipelineInfo& pipelineDesc) -> std::shared_ptr<Pipeline>;

		virtual ~Pipeline() = default;

		virtual auto getWidth()->uint32_t = 0;
		virtual auto getHeight()->uint32_t = 0;
		virtual auto getShader() const->std::shared_ptr<Shader> = 0;
		virtual auto bind(CommandBuffer * commandBuffer, uint32_t layer = 0, int32_t cubeFace = -1, int32_t mipMapLevel = 0)->FrameBuffer* = 0;
		virtual auto end(CommandBuffer * commandBuffer) -> void = 0;
		virtual auto clearRenderTargets(CommandBuffer * commandBuffer) -> void {};
	protected:
		PipelineInfo description;
	};
};