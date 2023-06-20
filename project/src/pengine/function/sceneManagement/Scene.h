#pragma once
#include <unordered_map>
#include <vector>
#include <memory>
#include <Queue>
#include <string>
#include "function/ecs/Entity/Entity.h"
#include "function/ecs/Entity/EntityManager.h"
#include "function/ecs/Component/Light.h"
#include "function/ecs/Component/Camera.h"

namespace pengine
{
	class RenderGraph;
	class  PENGINE_API Scene
	{
	public:
		Scene(const std::string &name);
		~Scene();
		auto cull() -> void;
		auto onUpdate() -> void;
		auto getRegistry() -> entt::registry&;
		auto createEntity() -> Entity;
		auto createEntity(const std::string& name) -> Entity;
		auto duplicateEntity(const Entity& entity, const Entity& parent) -> void;
		auto duplicateEntity(const Entity& entity) -> void;
		auto removeAllChildren(entt::entity entity) -> void;
	protected:
		std::vector<std::shared_ptr<Entity>> entitys;
		std::shared_ptr<EntityManager> entityManager;
		std::string                    name;
		std::function<void(Entity)> onEntityAdd;
		bool     dirty = false;
		std::shared_ptr <RenderGraph> renderGraph;

		uint32_t width = 0;
		uint32_t height = 0;
	};
}