#include "SceneGraph.h"
#include "core/log/PLog.h"
#include "OCT.h"
#include "function/ecs/Entity/EntityManager.h"
namespace pengine
{
	SceneGraph::SceneGraph(std::shared_ptr<EntityManager> _entityManager)
		: entityManager(_entityManager)
	{
	}
	auto SceneGraph::release() -> void
	{
	}
	auto SceneGraph::build(SceneGraphBuildFlag flag) -> void
	{
		if (flag == SceneGraphBuildFlag::OCTREE)
		{
			m_tree = std::make_shared<OCT>();
			m_tree->build(entityManager->getRegistry());
		}
		else
		{
			PLOGE(" Unsupport tree type !");
		}
	}
	auto SceneGraph::rebuild(SceneGraphBuildFlag flag) -> void
	{
	}
	auto SceneGraph::update()
	{
	}
	auto SceneGraph::addEntity()
	{
	}
	auto SceneGraph::removeEntity()
	{
	}
	auto SceneGraph::frustumCull(const Frustum& frustum, std::vector<entt::entity>& ents) -> void
	{
		m_tree->frustumCull(frustum, ents);
	}
};
