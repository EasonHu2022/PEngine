#include "EntityManager.h"
#include "function/ecs/component/Light.h"
#include "function/ecs/component/Camera.h"
#include "function/ecs/component/MeshRenderer.h"
#include "function/ecs/component/Transform.h"
#include "function/sceneManagement/Scene.h"
namespace pengine
{
	EntityManager::EntityManager()
	{
		//set constraint
		//attention : dependency means when add left one , automatically add the dependent one
		addDependency<component::Camera, component::Transform>();
		addDependency<component::Light, component::Transform>();
		addDependency<component::MeshRenderer, component::Transform>();
	}
	auto EntityManager::create() -> Entity
	{
		return Entity(registry.create(), getRegistry());
	}
	auto EntityManager::create(const std::string& name) -> Entity
	{
		auto e = registry.create();
		registry.emplace<component::NameComponent>(e, name);
		return Entity(e, getRegistry());
	}
	auto EntityManager::getEntityByName(const std::string& name) -> Entity
	{
		auto views = registry.view<component::NameComponent>();
		for (auto& view : views)
		{
			auto& comp = registry.get<component::NameComponent>(view);
			if (comp.name == name)
			{
				return { view, getRegistry() };
			}
		}
		return {};
	}
	auto EntityManager::clear() -> void
	{
		registry.each([&](auto entity) {
			registry.destroy(entity);
			});
		registry.clear();
	}
	auto EntityManager::removeAllChildren(entt::entity entity, bool root) -> void
	{
		auto hierarchyComponent = registry.try_get<component::Hierarchy>(entity);
		if (hierarchyComponent)
		{
			entt::entity child = hierarchyComponent->getFirst();
			while (child != entt::null)
			{
				auto hierarchyComponent = registry.try_get<component::Hierarchy>(child);
				auto next = hierarchyComponent ? hierarchyComponent->getNext() : entt::null;
				removeAllChildren(child, false);
				child = next;
			}
		}
		if (!root)
			registry.destroy(entity);
	}
	auto EntityManager::removeEntity(entt::entity entity) -> void
	{
		auto hierarchyComponent = registry.try_get<component::Hierarchy>(entity);
		if (hierarchyComponent)
		{
			entt::entity child = hierarchyComponent->getFirst();
			while (child != entt::null)
			{
				auto hierarchyComponent = registry.try_get<component::Hierarchy>(child);
				auto next = hierarchyComponent ? hierarchyComponent->getNext() : entt::null;
				removeEntity(child);
				child = next;
			}
		}
		registry.destroy(entity);
	}
};