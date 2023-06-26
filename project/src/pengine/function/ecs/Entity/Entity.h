#pragma once
#include <unordered_map>
#include <memory>
#include <typeinfo> 
#include <type_traits>
#include "function/ecs/component/Component.h"
#include <entt/entt.hpp>
namespace pengine
{
	class Entity
	{
	public:
		Entity() = default;

		Entity(entt::entity handle, entt::registry& initRegistry) :
			entityHandle(handle), registry(&initRegistry)
		{
		}
		~Entity()
		{
		}
		template<typename T, typename... Args>
		inline T& addComponent(Args &&...args)
		{
			if (hasComponent<T>())
				PLOGW("Attempting to add extisting component ");
			T& t = registry->emplace<T>(entityHandle, std::forward<Args>(args)...);
			if constexpr (std::is_base_of<component::Component, T>::value)
			{
				t.setEntity(entityHandle);
			}
			return t;
		}

		template <typename T, typename... Args>
		inline T& getOrAddComponent(Args &&...args)
		{
			T& t = registry->get_or_emplace<T>(entityHandle, std::forward<Args>(args)...);
			if constexpr (std::is_base_of<component::Component, T>::value)
			{
				t.setEntity(entityHandle);
			}
			return t;
		}

		template <typename T, typename... Args>
		inline auto addOrReplaceComponent(Args &&...args)
		{
			T& t = registry->emplace_or_replace<T>(entityHandle, std::forward<Args>(args)...);
			if constexpr (std::is_base_of<component::Component, T>::value)
			{
				t.setEntity(entityHandle);
			}
		}

		template <typename T>
		inline T& getComponent()
		{
			return registry->get<T>(entityHandle);
		}

		template <typename T>
		inline T* tryGetComponent()
		{
			return registry->try_get<T>(entityHandle);
		}

		template <typename T>
		inline T* tryGetComponentFromParent()
		{
			auto t = registry->try_get<T>(entityHandle);
			if (t == nullptr)
			{
				auto parent = getParent();
				while (parent.valid() && t == nullptr)
				{
					t = parent.tryGetComponent<T>();
					parent = parent.getParent();
				}
			}
			return t;
		}

		template <typename T>
		inline auto hasComponent() const -> bool
		{
			return registry->try_get<T>(entityHandle) != nullptr ? true : false;
		}

		template <typename T>
		inline auto removeComponent()
		{
			return registry->remove<T>(entityHandle);
		}

		auto isActive() -> bool;
		auto setActive(bool active) -> void;
		auto setParent(const Entity& entity) -> void;
		auto findByPath(const std::string& path) -> Entity;
		auto getChildInChildren(const std::string& name) -> Entity;
		auto getParent() -> Entity;
		auto getChildren() -> std::vector<Entity>;
		auto removeAllChildren() -> void;
		auto isParent(const Entity& potentialParent) const -> bool;
		auto destroy() -> void;
		auto valid() -> bool;

		inline operator entt::entity() const
		{
			return entityHandle;
		}
		inline operator uint32_t() const
		{
			return (uint32_t)entityHandle;
		}

		inline operator bool() const
		{
			return entityHandle != entt::null;
		}

		inline auto operator==(const Entity& other) const
		{
			return entityHandle == other.entityHandle;
		}

		inline auto operator!=(const Entity& other) const
		{
			return !(*this == other);
		}

		inline auto getHandle() const
		{
			return entityHandle;
		}
		inline auto setHandle(entt::entity en)
		{
			entityHandle = en;
		}
	protected:
		entt::entity    entityHandle = entt::null;
		entt::registry* registry = nullptr;
	};
}