#pragma once
#include "IPass.h"
namespace pengine
{
	class RenderVoxelizationPass : IPass
	{
	public:
		RenderVoxelizationPass(uint32_t _uid, RenderGraph* renderGraph);
		~RenderVoxelizationPass();
		auto init(entt::registry& registry) -> void override;
		//record
		auto execute(CommandBuffer* commandBuffer) -> void override;
		auto setup() -> void override;
		auto onUpdate(entt::registry& registry, std::vector<entt::entity>& culledEnts) -> void override;
		auto onResize(uint32_t width, uint32_t height, uint32_t displayWidth, uint32_t displayHeight) -> void override;
		auto createVResource() -> void;
	};
};