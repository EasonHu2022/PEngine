#include "RenderDeferredLightingTask.h"
#include "RenderGraph.h"
namespace pengine
{
	RenderDeferredLightingTask::RenderDeferredLightingTask(uint32_t uid, RenderGraph* renderGraph) : ITask(uid, renderGraph)
	{
	}
	RenderDeferredLightingTask::~RenderDeferredLightingTask()
	{
	}
	auto RenderDeferredLightingTask::init(entt::registry& registry) -> void
	{
	}
	auto RenderDeferredLightingTask::execute(CommandBuffer* commandBuffer) -> void
	{
	}
	auto RenderDeferredLightingTask::setup() -> void
	{
	}
	auto RenderDeferredLightingTask::onResize(uint32_t width, uint32_t height, uint32_t displayWidth, uint32_t displayHeight) -> void
	{
	}
};