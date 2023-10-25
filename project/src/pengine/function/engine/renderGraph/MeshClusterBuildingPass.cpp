#include "MeshClusterBuildingPass.h"
#include "function/ecs/component/Camera.h"
#include "function/ecs/component/Transform.h"
#include "function/ecs/component/MeshRenderer.h"
namespace pengine
{
	MeshClusterBuildingPass::MeshClusterBuildingPass(uint32_t _uid, RenderGraph* renderGraph) : IPass(_uid, renderGraph)
	{
	}
	MeshClusterBuildingPass::~MeshClusterBuildingPass()
	{
	}
	auto MeshClusterBuildingPass::init(entt::registry& registry) -> void
	{
		
	}
	auto MeshClusterBuildingPass::execute(CommandBuffer* commandBuffer) -> void
	{
		
	}
	auto MeshClusterBuildingPass::setup() -> void
	{
		
	}
	auto MeshClusterBuildingPass::onUpdate(entt::registry& registry, std::vector<entt::entity>& culledEnts) -> void
	{
		auto cameras = registry.group<component::Camera>(entt::get<component::Transform>);
		if (cameras.empty())
		{
			PLOGW("Non Main Camera Detected!");
			return;
		}
		if (cameras.size() > 1)
		{
			PLOGW("More than one Main Camera Detected! Random one will be used !");
		}
		auto cameraEnt = Entity(cameras.front(), registry);
		auto& cameraTransform = cameraEnt.getComponent<component::Transform>();
		auto& cameraComp = cameraEnt.getComponent<component::Camera>();
		auto meshes = registry.group<component::MeshRenderer>(entt::get< component::Transform>);
		if (!meshes.empty())
		{
			for (const auto& data : meshes.each())
			{
				auto& [ent, meshRenderer, transform] = data;
				auto mesh = meshRenderer.getMesh().get();

			}			
		}
	}
	auto MeshClusterBuildingPass::onResize(uint32_t width, uint32_t height, uint32_t displayWidth, uint32_t displayHeight) -> void
	{
		
	}
	auto MeshClusterBuildingPass::createVResource() -> void
	{
	}
}; 