#pragma once
#include "ModelLoader.h"

namespace pengine
{
	class FBXLoader : public ModelLoader
	{
	public:
		static constexpr char* EXTENSIONS[] = { "fbx" };
		auto load(const std::string& obj, const std::string& extension, std::unordered_map<std::string,
			std::shared_ptr<Mesh>>&, std::shared_ptr<Skeleton>& skeleton) -> void;
	};
};