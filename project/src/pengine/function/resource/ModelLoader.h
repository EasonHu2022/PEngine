#pragma once
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include "core/core.h"
#include "function/engine/Mesh.h"
namespace pengine
{

	class Skeleton;

	namespace ModelLoaderProxy
	{
		auto PENGINE_API load(const std::string& obj, std::unordered_map<std::string, std::shared_ptr<Mesh>>&, std::shared_ptr<Skeleton>& skeleton) -> void;
	};

	class PENGINE_API ModelLoader
	{
	public:
		virtual auto load(const std::string& obj, const std::string& extension, std::unordered_map<std::string,
			std::shared_ptr<Mesh>>&, std::shared_ptr<Skeleton>& skeleton) -> void = 0;
		virtual auto release() -> void = 0;
	};

	class PENGINE_API ModelLoaderFactory
	{
	public:
		ModelLoaderFactory();
		template<typename T>
		auto addModelLoader() -> void;
		auto release() -> void;
		auto load(const std::string& obj,
			std::unordered_map<std::string, std::shared_ptr<Mesh>>& meshes,
			std::shared_ptr<Skeleton>& skeleton) -> void;
		inline auto& getSupportExtensions() const {
			return supportExtensions;
		}
	private:
		std::unordered_map<std::string, std::shared_ptr<ModelLoader>> loaders;
		std::unordered_set<std::string> supportExtensions;
	};

	template<typename T>
	inline auto ModelLoaderFactory::addModelLoader() -> void
	{
		auto loader = std::make_shared<T>();
		for (auto ext : T::EXTENSIONS)
		{
			loaders.emplace(ext, loader);
			supportExtensions.emplace(ext);
		}
	}

};