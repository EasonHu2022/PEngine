#pragma once
#include "core/core.h"
#include <entt/entt.hpp>
namespace pengine
{
	class Entity;
	namespace component
	{
		class PENGINE_API Component
		{
		public:
			virtual ~Component() = default;
			auto  getEntity() -> pengine::Entity;
			inline auto& getEntityId() const
			{
				return entity;
			}
			inline auto& getEntityId()
			{
				return entity;
			}
			auto setEntity(entt::entity entity) -> void;
		protected:
			entt::entity entity = entt::null;
		};

		class PENGINE_API NameComponent : public Component
		{
		public:
			NameComponent() = default;

			NameComponent(const std::string& name) :
				name(name)
			{}
			std::string name;
		};

		class PENGINE_API ActiveComponent : public Component
		{
		public:
			ActiveComponent() = default;
			ActiveComponent(bool active) :
				active(active)
			{}
			bool active = true;
		};

		class PENGINE_API Hierarchy : public Component
		{
		public:
			Hierarchy(entt::entity p);
			Hierarchy();

			inline auto getParent() const
			{
				return parent;
			}
			inline auto getNext() const
			{
				return next;
			}
			inline auto getPrev() const
			{
				return prev;
			}
			inline auto getFirst() const
			{
				return first;
			}

			// Return true if current entity is an ancestor of current entity
			//seems that the entt as a reference could have a bug....
			//TODO
			auto compare(const entt::registry& registry, entt::entity entity) const -> bool;
			auto reset() -> void;

			//delegate method
			// update hierarchy components when hierarchy component is added
			static auto onConstruct(entt::registry& registry, entt::entity entity) -> void;
			static auto onDestroy(entt::registry& registry, entt::entity entity) -> void;
			static auto onUpdate(entt::registry& registry, entt::entity entity) -> void;

			//adjust the parent
			static auto reparent(entt::entity entity, entt::entity parent, entt::registry& registry, Hierarchy& hierarchy) -> void;

			entt::entity parent = entt::null;
			entt::entity first = entt::null;
			entt::entity next = entt::null;
			entt::entity prev = entt::null;
		};
	}
	
}