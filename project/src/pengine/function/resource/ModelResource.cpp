#include "ModelResource.h"
#
#include "ModelLoader.h"

namespace pengine
{
	ModelResource::ModelResource(const std::string& name) :name(name)
	{
		ModelLoaderProxy::load(name, meshes, skeleton);
	}
	auto ModelResource::addMesh(const std::string& name, Mesh* mesh) -> void
	{
		meshes.emplace(name, std::shared_ptr<Mesh>(mesh));
	}
	auto ModelResource::addMesh(const std::string& name, std::shared_ptr<Mesh> mesh) -> void
	{
		meshes.emplace(name, mesh);
	}
	auto ModelResource::find(const std::string& name) -> std::shared_ptr<Mesh>
	{
		if (auto iter = meshes.find(name); iter != meshes.end()) {
			return iter->second;
		}
		return nullptr;
	}
};