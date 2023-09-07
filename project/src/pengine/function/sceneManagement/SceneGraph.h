#pragma once	
#include <memory>
#include <vector>
#include "glm/glm.hpp"
#include "entt/entt.hpp"
namespace pengine
{
	enum SceneGraphBuildFlag
	{
		BVH = 0,//better performance for ray tracing
		OCTREE,// stable, dynamic objects
		BSP,
	};
	class Frustum;
	class EntityManager;
	class SceneTree;
	class BoundingBox;
	class SceneGraph
	{
	public:
		SceneGraph(std::shared_ptr<EntityManager> entityManager);
		~SceneGraph() = default;
		auto release() -> void;
		auto build(SceneGraphBuildFlag flag = SceneGraphBuildFlag::OCTREE) -> void;
		auto rebuild(SceneGraphBuildFlag flag = SceneGraphBuildFlag::OCTREE) -> void;
		auto update();
		auto addEntity();
		auto removeEntity();
		auto frustumCull(const Frustum& frustum, std::vector<entt::entity> &ents) -> void;
		auto getSceneBoundingBox() -> std::shared_ptr<BoundingBox>;
	private:
		std::shared_ptr<EntityManager> entityManager;
		std::shared_ptr<SceneTree> m_tree;
	};
};