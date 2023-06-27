#pragma once
#include "IResource.h"
#include "core/HashCode.h"
#include <memory>
#include <string>
#include <unordered_map>

namespace pengine
{
	class Cache
	{
	  public:
		friend class Material;

		virtual ~Cache() = default;

		template <typename T, typename... Args>
		inline auto emplace(Args &&...args) -> std::shared_ptr<T>
		{
			static_assert(std::is_base_of<IResource, T>::value, "class T should extend from IResource");

			size_t hash = 0;
			HashCode::hashCode(hash, args...);
			auto iter = cache.find(hash);
			if (iter == cache.end())
			{
				iter = cache.emplace(hash, std::make_shared<T>(std::forward<Args>(args)...))
				           .first;
			}
			return std::static_pointer_cast<T>(iter->second);
		}

		inline auto& getCache() const
		{
			return cache;
		}
	
	  private:
		std::unordered_map<size_t, std::shared_ptr<IResource>> cache;
	};

};      
