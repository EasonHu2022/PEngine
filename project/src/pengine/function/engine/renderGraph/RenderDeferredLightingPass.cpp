#include "RenderDeferredLightingPass.h"
#include "RenderGraph.h"
namespace pengine
{
	RenderDeferredLightingPass::RenderDeferredLightingPass(uint32_t uid, RenderGraph* renderGraph) : IPass(uid, renderGraph)
	{
	}
	RenderDeferredLightingPass::~RenderDeferredLightingPass()
	{
	}
	auto RenderDeferredLightingPass::init(entt::registry& registry) -> void
	{
	}
	auto RenderDeferredLightingPass::execute(CommandBuffer* commandBuffer) -> void
	{
	}
	auto RenderDeferredLightingPass::setup() -> void
	{
	}
	auto RenderDeferredLightingPass::onUpdate(entt::registry& registry) -> void 
	{
		
	}
	auto RenderDeferredLightingPass::onResize(uint32_t width, uint32_t height, uint32_t displayWidth, uint32_t displayHeight) -> void
	{
	}
};