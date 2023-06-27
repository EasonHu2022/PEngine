#pragma once
#include "ModelLoader.h"
namespace pengine
{
	class GLTFLoader : public ModelLoader
	{
	public:
		static constexpr char* EXTENSIONS[] = { "gltf","glb" };
		auto load(const std::string& obj, const std::string& extension, std::unordered_map<std::string,
			std::shared_ptr<Mesh>>&, std::shared_ptr<Skeleton>& skeleton) -> void;
	};

};