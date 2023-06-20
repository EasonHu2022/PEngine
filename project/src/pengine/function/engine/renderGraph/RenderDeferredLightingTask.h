#pragma once
#include "ITask.h"
namespace pengine
{
	class RenderDeferredLightingTask : public ITask
	{
	public:
		RenderDeferredLightingTask(uint32_t uid, RenderGraph* renderGraph);
		~RenderDeferredLightingTask();
		auto init(entt::registry& registry) -> void override;
		auto execute(CommandBuffer* commandBuffer) -> void override;
		auto setup() -> void override;
		auto onUpdate(entt::registry& registry) -> void override;
		auto onResize(uint32_t width, uint32_t height, uint32_t displayWidth, uint32_t displayHeight) -> void;
		auto onSceneElementChange(Entity& ent) -> void;
	private:
	};
};