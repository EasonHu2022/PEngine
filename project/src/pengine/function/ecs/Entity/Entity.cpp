#include "Entity.h"
#include "core/utils/StringUtils.h"
namespace pengine
{
	auto Entity::isActive() -> bool
	{
		if (hasComponent<component::ActiveComponent>())
			return registry->get<component::ActiveComponent>(entityHandle).active;
		return false;
	}
	auto Entity::setActive(bool active) -> void
	{
		getOrAddComponent<component::ActiveComponent>().active = isActive;
	}
	auto Entity::setParent(const Entity& entity) -> void
	{
		bool acceptable = false;
		auto hierarchyComponent = tryGetComponent<component::Hierarchy>();
		if (hierarchyComponent != nullptr)
		{
			//not self, not already self, not loop
			acceptable = entity.entityHandle != entityHandle && (!entity.isParent(*this)) && (hierarchyComponent->getParent() != entityHandle);
		}
		else
			acceptable = entity.entityHandle != entityHandle;

		if (!acceptable)
			return;

		if (hierarchyComponent)
			component::Hierarchy::reparent(entityHandle, entity.entityHandle, *registry, *hierarchyComponent);
		else
		{
			registry->emplace<component::Hierarchy>(entityHandle, entity.entityHandle);
		}
	}
	auto Entity::findByPath(const std::string& path) -> Entity
	{
		if (path == "")
		{
			return {};
		}
		Entity ent = {};
		auto   layers = StringUtils::split(path, "/");
		auto   children = getChildren();

		for (int32_t i = 0; i < layers.size(); ++i)
		{
			bool findChild = false;

			if (layers[i] == "..")
			{
				ent = getParent();
			}
			else
			{
				for (auto entt : children)
				{
					auto& nameComp = entt.getComponent<component::NameComponent>();
					if (layers[i] == nameComp.name)
					{
						ent = entt;
						children = ent.getChildren();
						break;
					}
				}
			}
		}
		return ent;
	}
	auto Entity::getChildInChildren(const std::string& name) -> Entity
	{
		auto children = getChildren();
		for (auto entt : children)
		{
			auto& nameComp = entt.getComponent<component::NameComponent>();
			if (name == nameComp.name)
			{
				return entt;
			}
			auto ret = entt.getChildInChildren(name);
			if (ret.valid())
			{
				return ret;
			}
		}
		return {};
	}
	auto Entity::getParent() -> Entity
	{
		auto hierarchyComp = tryGetComponent<component::Hierarchy>();
		if (hierarchyComp)
			return Entity(hierarchyComp->getParent(), *registry);
		else
			return {};
	}
	auto Entity::getChildren() -> std::vector<Entity>
	{
		std::vector<Entity> children;
		auto hierarchyComponent = tryGetComponent<component::Hierarchy>();
		if (hierarchyComponent)
		{
			entt::entity child = hierarchyComponent->getFirst();
			while (child != entt::null && registry->valid(child))
			{
				children.emplace_back(child, *registry);
				hierarchyComponent = registry->try_get<component::Hierarchy>(child);
				if (hierarchyComponent)
					child = hierarchyComponent->getNext();
			}
		}
		return children;
	}
	auto Entity::removeAllChildren() -> void
	{
		auto hierarchyComponent = registry->try_get<component::Hierarchy>(entityHandle);
		if (hierarchyComponent)
		{
			entt::entity child = hierarchyComponent->getFirst();
			while (child != entt::null)
			{
				auto   hierarchyComponent = registry->try_get<component::Hierarchy>(child);
				auto   next = hierarchyComponent ? hierarchyComponent->getNext() : entt::null;
				Entity ent = { child, *registry };
				ent.removeAllChildren();
				ent.destroy();
				child = next;
			}
		}
	}
	auto Entity::isParent(const Entity& potentialParent) const -> bool
	{
		auto nodeHierarchyComponent = registry->try_get<component::Hierarchy>(entityHandle);
		if (nodeHierarchyComponent)
		{
			auto parent = nodeHierarchyComponent->getParent();
			while (parent != entt::null)
			{
				if (parent == potentialParent.entityHandle)
				{
					return true;
				}
				else
				{
					nodeHierarchyComponent = registry->try_get<component::Hierarchy>(parent);
					parent = nodeHierarchyComponent ? nodeHierarchyComponent->getParent() : entt::null;
				}
			}
		}
		return false;
	}
	auto Entity::destroy() -> void
	{
		registry->destroy(entityHandle);
	}
	auto Entity::valid() -> bool
	{
		return registry && registry->valid(entityHandle);
	}
};