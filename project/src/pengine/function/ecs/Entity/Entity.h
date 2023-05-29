#pragma once
#include <unordered_map>
#include <memory>
#include <typeinfo> 
#include <type_traits>
#include "function/ecs/component/Component.h"
namespace pengine
{
	class Entity
	{
	public:
		template<typename T, typename... Args>
		auto addComponent(Args &&...args)->std::shared_ptr<T>
		{
			static_assert(std::is_base_of<Component, T>::value, "addComponent class T should extend from Component");
			return components.emplace(typeid(T).hash_code(), std::make_shared<T>(std::forward<Args>(args)...));
		}

		template<typename T>
		auto getComponent() -> std::shared_ptr<T>
		{
			auto it = components.find(typeid(T).hash_code());
			if (it != components.end()) {
				return dynamic_cast<std::shared_ptr<T>>(it->second);
			}
			return NULL;
		}

		template<typename T>
		auto removeComponent() -> void
		{
			auto it = components.find(typeid(T).hash_code());
			if (it != components.end())
			{
				components.erase(it);
			}
		}

	private:
		std::unordered_map<size_t, std::shared_ptr<Component>> components;
		
	};
}