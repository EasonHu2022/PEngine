#include "OCT.h"
#include "function/ecs/component/Transform.h"
#include "function/ecs/component/MeshRenderer.h"
#include "function/ecs/Entity/Entity.h"
#include "function/engine/Frustum.h"
#include "function/profile/profiler.h"
namespace pengine
{
	OCT::OCT()
	{
	}
	OCT::~OCT()
	{
	}
	void OCT::build(entt::registry& registry)
	{
		//PROFILE_FUNCTION();
		//create rootNode
		rootNode = std::make_shared<SceneTreeNode>();
		rootNode->depth = 0;
		auto boundingBox = std::make_shared<BoundingBox>();
		//only concern about meshrenderer and skinmeshrenderer so.
		auto viewsMeshRenderer = registry.group < component::MeshRenderer >(entt::get<component::Transform>);
		if (!viewsMeshRenderer.empty())
		{
			for (const auto& data : viewsMeshRenderer.each())
			{
				rootNode->entHandles.push_back(std::get<0>(data));
				Entity ent(std::get<0>(data), registry);
				auto& transform = ent.getComponent<component::Transform>();
				auto mesh = ent.getComponent<component::MeshRenderer>().getMesh().get();// to entity level is enough
				boundingBox->merge(mesh->getBoundingBox());
			}
			rootNode->boundingBox = boundingBox;
		}
		recursiveBuild(rootNode, registry);
	}
	void OCT::update()
	{
	}
	void OCT::addEntity(entt::entity ent)
	{
	}
	void OCT::removeEntity(entt::entity ent)
	{
	
	}
	void OCT::clear()
	{
		rootNode.reset();
	}
	void OCT::rebuild(entt::registry& reg)
	{
		clear();
		build(reg);
	}

	void OCT::getInside(std::shared_ptr<SceneTreeNode> node, const Frustum& frustum,std::vector<entt::entity>& ent)
	{
		if (!frustum.isInside(node->boundingBox))
			return;
		if (node->children.size() == 0)//leaf
		{
			for (auto entid : node->entHandles)
			{
				ent.push_back(entid);
			}		
			return;
		}
		for (auto &child : node->children)
		{
			getInside(child,frustum,ent);
		}

	}
	void OCT::frustumCull(const Frustum& frustum, std::vector<entt::entity>& out_ent)
	{
		//PROFILE_FUNCTION();
		getInside(rootNode, frustum, out_ent);
	}
	std::shared_ptr<BoundingBox> OCT::getBoundingBox()
	{
		return rootNode->boundingBox;
	}
	void OCT::recursiveBuild(std::shared_ptr<SceneTreeNode> node, entt::registry& registry)
	{
		if (node->depth >= maxDepth || node->entHandles.size() < 2) return;
		for (int i = 0; i < 8; i++)
		{
			auto child = std::make_shared<SceneTreeNode>();
			child->boundingBox = std::make_shared<BoundingBox>();
			child->depth = node->depth + 1;
			node->children.push_back(child);
		}
		
		auto middle = node->boundingBox->center();
		auto min = node->boundingBox->min;
		auto max = node->boundingBox->max;
		auto bbltf = new BoundingBox(glm::vec3(min.x, middle.y, min.z), glm::vec3(middle.x, max.y, middle.z));//0
		auto bbrtf = new BoundingBox(glm::vec3(middle.x, middle.y, min.z), glm::vec3(max.x, max.y, middle.z));//1
		auto bbltb = new BoundingBox(glm::vec3(min.x, middle.y, middle.z), glm::vec3(middle.x, max.y, max.z));//2
		auto bbrtb = new BoundingBox(glm::vec3(middle.x, middle.y, middle.z), glm::vec3(max.x, max.y, max.z));//3
		auto bblbf = new BoundingBox(glm::vec3(min.x, min.y, min.z), glm::vec3(middle.x, middle.y, middle.z));//4
		auto bbrbf = new BoundingBox(glm::vec3(middle.x, min.y, min.z), glm::vec3(max.x, middle.y, middle.z));//5
		auto bblbb = new BoundingBox(glm::vec3(min.x, min.y, middle.z), glm::vec3(middle.x, middle.y, max.z));//6
		auto bbrbb = new BoundingBox(glm::vec3(middle.x, min.y, middle.z), glm::vec3(max.x, middle.y, max.z));//7
		for (auto id : node->entHandles)
		{
			Entity ent(id, registry);
			auto bb = ent.getComponent<component::MeshRenderer>().getMesh()->getBoundingBox();
			auto center = bb->center();
			if (bbltf->contains(center))
			{
				node->children[0]->entHandles.push_back(id);
				node->children[0]->boundingBox->merge(bb);
				continue;
			}
			if (bbrtf->contains(center))
			{
				node->children[1]->entHandles.push_back(id);
				node->children[1]->boundingBox->merge(bb);
				continue;
			}
			if (bbltb->contains(center))
			{
				node->children[2]->entHandles.push_back(id);
				node->children[2]->boundingBox->merge(bb);
				continue;
			}
			if (bbrtb->contains(center))
			{
				node->children[3]->entHandles.push_back(id);
				node->children[3]->boundingBox->merge(bb);
				continue;
			}
			if (bblbf->contains(center))
			{
				node->children[4]->entHandles.push_back(id);
				node->children[4]->boundingBox->merge(bb);
				continue;
			}
			if (bbrbf->contains(center))
			{
				node->children[5]->entHandles.push_back(id);
				node->children[5]->boundingBox->merge(bb);
				continue;
			}
			if (bblbb->contains(center))
			{
				node->children[6]->entHandles.push_back(id);
				node->children[6]->boundingBox->merge(bb);
				continue;
			}
			if (bbrbb->contains(center))
			{
				node->children[7]->entHandles.push_back(id);
				node->children[7]->boundingBox->merge(bb);
				continue;
			}
		}
		for (auto child : node->children)
		{
			recursiveBuild(child, registry);
		}
	}
};