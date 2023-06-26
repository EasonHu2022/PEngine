#include "Component.h"
#include "Application.h"
#include "function/sceneManagement/Scene.h"
#include "function/ecs/Entity/Entity.h"
namespace pengine
{
	namespace component
	{
		pengine::Entity Component::getEntity()
		{
			return { entity, Application::get()->getSceneManager()->getCurrentScene()->getRegistry()};
		}
		auto Component::setEntity(entt::entity entity) -> void
		{
			this->entity = entity;
		}
		Hierarchy::Hierarchy(entt::entity p)
		{
		}
		Hierarchy::Hierarchy()
		{
		}
		auto Hierarchy::compare(const entt::registry& registry, entt::entity entity) const -> bool
		{
			return false;
		}
		auto Hierarchy::reset() -> void
		{
		}
		auto Hierarchy::onConstruct(entt::registry& registry, entt::entity entity) -> void
		{
		}
		auto Hierarchy::onDestroy(entt::registry& registry, entt::entity entity) -> void
		{
		}
		auto Hierarchy::onUpdate(entt::registry& registry, entt::entity entity) -> void
		{
		}
		auto Hierarchy::reparent(entt::entity entity, entt::entity parent, entt::registry& registry, Hierarchy& hierarchy) -> void
		{
		}
	};
};