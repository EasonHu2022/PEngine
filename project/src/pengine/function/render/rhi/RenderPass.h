#pragma once
#include "Defs.h"
namespace pengine
{
	class RenderPass
	{
	public:
		virtual ~RenderPass() = default;
		static auto create(const RenderPassInfo & renderPassDesc)->std::shared_ptr<RenderPass>;

		virtual auto getAttachmentCount() const->int32_t = 0;
		virtual auto beginRenderPass(CommandBuffer * commandBuffer, const glm::vec4 & clearColor, FrameBuffer * frame, SubPassContents contents, uint32_t width, uint32_t height, int32_t cubeFace = -1, int32_t mipMapLevel = 0) const -> void = 0;
		virtual auto endRenderPass(CommandBuffer* commandBuffer) -> void = 0;
	};


};