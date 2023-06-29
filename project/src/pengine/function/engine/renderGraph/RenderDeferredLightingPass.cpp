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
		createVResource();
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
	auto RenderDeferredLightingPass::createVResource() -> void
	{
		inputs.resize(5);
		outputs.resize(7);
		//temp all the same
		for (int i = 0; i < inputs.size(); i++)
		{
			auto res = new RenderGraphVirtualResource();
			res->type = RenderResouceType::Res_Texture2D;
			res->format = TextureFormat::RGBA16;
			auto renderExtend = m_renderGraph->getRenderExtend();
			res->width = renderExtend.x;
			res->height = renderExtend.y;
			inputs[i] = std::shared_ptr<RenderGraphVirtualResource>(res);
		}
		for (int i = 0; i < outputs.size(); i++)
		{
			auto res = new RenderGraphVirtualResource();
			res->type = RenderResouceType::Res_Texture2D;
			res->format = TextureFormat::RGBA16;
			auto renderExtend = m_renderGraph->getRenderExtend();
			res->width = renderExtend.x;
			res->height = renderExtend.y;
			outputs[i] = std::shared_ptr<RenderGraphVirtualResource>(res);
		}
	}
};