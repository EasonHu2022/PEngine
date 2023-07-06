#include "ISystem.h"
#include <memory>
#include <unordered_map>
#include "core/core.h"
#include <typeinfo>
namespace pengine
{
	class Scene;
	class SystemManager
	{
	public:
		template <typename T, typename... Args>
		auto addSystem(Args &&...args) -> std::shared_ptr<T>
		{
			static_assert(std::is_base_of<ISystem, T>::value, "class T should extend from ISystem");
			auto typeName = typeid(T).hash_code();
			PENGINE_ASSERT(systems.find(typeName) == systems.end(), "Add system more than once.");
			PLOGI("Add System: {0}", typeid(T).name());
			return std::static_pointer_cast<T>(systems.emplace(typeName, std::make_shared<T>(std::forward<Args>(args)...)).first->second);
		}
		/// <summary>
		/// no param constructor
		/// </summary>
		/// <typeparam name="T"></typeparam>
		/// <returns></returns>
		template <typename T>
		auto addSystem(T* t) -> std::shared_ptr<T>
		{
			static_assert(std::is_base_of<ISystem, T>::value, "class T should extend from ISystem");
			auto typeName = typeid(T).hash_code();
			PENGINE_ASSERT(systems.find(typeName) == systems.end(), "Add system more than once.");
			PLOGI("Add System: {0}", typeid(T).name());
			return std::static_pointer_cast<T>(systems.emplace(typeName, std::shared_ptr<T>(t)).first->second);
		}

		template <typename T>
		auto removeSystem() -> void
		{
			auto typeName = typeid(T).hash_code();

			if (systems.find(typeName) != systems.end())
			{
				systems.erase(typeName);
			}
		}

		template <typename T>
		auto getSystem() -> T*
		{
			auto typeName = typeid(T).hash_code();
			if (systems.find(typeName) != systems.end())
			{
				return dynamic_cast<T*>(systems[typeName].get());
			}
			return nullptr;
		}

		template <typename T>
		auto hasSystem() -> T*
		{
			auto typeName = typeid(T).hash_code();
			return systems.find(typeName) != systems.end();
		}


		auto onInit()-> void;
		auto onImGui()-> void;
		auto onUpdate(float dt, Scene* scene) -> void ;
		auto release() -> void;
	private:
		std::unordered_map<size_t, std::shared_ptr<ISystem>> systems;
	};

	
};