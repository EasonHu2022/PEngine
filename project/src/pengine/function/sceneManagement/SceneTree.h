#pragma once	
#include "entt/entt.hpp"
#include "glm/glm.hpp"
#include "function/engine/BoundingBox.h"
namespace pengine
{
	struct SceneTreeNode
	{
		uint32_t depth;
		std::vector<entt::entity> entHandles;
		std::shared_ptr<BoundingBox> boundingBox;
		std::vector<std::shared_ptr<SceneTreeNode>> children;
	};
	class Frustum;
	class BoundingBox;
	class SceneTree
	{
	public:
		SceneTree() = default;
		virtual ~SceneTree() = default;
		virtual void build(entt::registry& rig) = 0;
		virtual void update() = 0;
		virtual void addEntity(entt::entity ent) = 0;
		virtual void removeEntity(entt::entity ent) = 0;
		virtual void clear() = 0;
		virtual void rebuild(entt::registry& reg) = 0;
		virtual void getInside(std::shared_ptr<SceneTreeNode> node, const Frustum& frustum,std::vector<entt::entity>& ent) = 0;
		virtual void frustumCull(const Frustum& frustum, std::vector<entt::entity>& out_ent) = 0;
		virtual std::shared_ptr<BoundingBox> getBoundingBox() = 0;
	protected:
		std::shared_ptr<SceneTreeNode> rootNode;
		uint32_t maxDepth = 8;
	};
}