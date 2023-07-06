#include "ModelLoader.h"
#include "GLTFLoader.h"
#include "OBJLoader.h"
#include "FBXLoader.h"
#include "core/utils/StringUtils.h"
#include "Application.h"
namespace pengine
{
	ModelLoaderFactory::ModelLoaderFactory()
	{
		addModelLoader<GLTFLoader>();
		addModelLoader<OBJLoader>();
		addModelLoader<FBXLoader>();
	}
	auto ModelLoaderFactory::release() -> void
	{
		for (auto& ld : loaders)
		{
			ld.second->release();
		}
		loaders.clear();
	}
	auto ModelLoaderFactory::load(const std::string& obj, std::unordered_map<std::string, std::shared_ptr<Mesh>>& meshes, std::shared_ptr<Skeleton>& skeleton) -> void
	{
		auto extension = StringUtils::getExtension(obj);

		if (auto loader = loaders.find(extension); loader != loaders.end())
		{
			loader->second->load(obj, extension, meshes, skeleton);
		}
		else
		{
			PLOGE("Unknown file extension {0}", extension);
		}
	}

	namespace ModelLoaderProxy
	{
		auto PENGINE_API load(const std::string& obj, std::unordered_map<std::string, std::shared_ptr<Mesh>>& meshes, std::shared_ptr<Skeleton>& skeleton) -> void
		{
			Application::getLoaderFactory()->load(obj, meshes, skeleton);
		}
	};
};


