#pragma once
#include <string>
#include <memory>
#include "IResource.h"
namespace pengine
{
	class Mesh;
	class Skeleton;
	class ModelResource : public IResource
	{
	public:
		ModelResource(const std::string & name);
		auto addMesh(const std::string& name, Mesh* mesh) -> void;
		auto addMesh(const std::string& name, std::shared_ptr<Mesh> mesh) -> void;
		auto find(const std::string& name) -> std::shared_ptr<Mesh>;
		inline auto& getMeshes() const
		{
			return meshes;
		}
		auto getResourceType() const -> FileType override
		{
			return FileType::Model;
		};
		auto getPath() const -> std::string override
		{
			return name;
		};

		inline auto getSkeleton()
		{
			return skeleton;
		}
	private:
		std::unordered_map<std::string, std::shared_ptr<Mesh>> meshes;
		std::string                                            name;
		std::shared_ptr<Skeleton>							   skeleton;

	};
};