#pragma once
#include "SceneTree.h"
namespace pengine
{
	class OCT : public SceneTree
	{
	public:
		OCT();
		~OCT();
		void build(entt::registry& reg) override;
		void update() override;
		void addEntity(entt::entity ent) override;
		void removeEntity(entt::entity ent) override;
		void clear() override;
		void rebuild(entt::registry& reg) override;
		void getInside(std::shared_ptr<SceneTreeNode> node, const Frustum& frustum,std::vector<entt::entity>& ent) override;
		void frustumCull(const Frustum& frustum, std::vector<entt::entity>& out_ent) override;
		std::shared_ptr<BoundingBox> getBoundingBox() override;
	private:
		void recursiveBuild(std::shared_ptr<SceneTreeNode> node, entt::registry& registry);
	};
}