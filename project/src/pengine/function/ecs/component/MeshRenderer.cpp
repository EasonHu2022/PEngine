#include "MeshRenderer.h"
#include "Application.h"
#include "function/sceneManagement/Scene.h"
namespace pengine
{
	namespace component
	{
		MeshRenderer::MeshRenderer(const std::shared_ptr<pengine::Mesh>& meshes) : mesh(meshes)
		{
		}

		auto MeshRenderer::isActive() const -> bool
		{
			return mesh ? mesh->isActive() : false;
		}

		auto MeshRenderer::getMesh(const std::string& name) -> void
		{
			auto currentScene = Application::get()->getSceneManager()->getCurrentScene();
			Entity ent{ entity, currentScene->getRegistry() };
			auto   model = ent.tryGetComponent<Model>();
			if (model != nullptr)
			{
				switch (model->type)
				{
				case PrimitiveType::Cube:
					mesh = Mesh::createCube();
					break;
				case PrimitiveType::Sphere:
					mesh = Mesh::createSphere();
					break;
				case PrimitiveType::File:
					mesh = model->resource->find(name);
					break;
				case PrimitiveType::Pyramid:
					mesh = Mesh::createPyramid();
					break;
				}
			}
			else
			{
				model = ent.tryGetComponentFromParent<Model>();
				mesh = model->resource->find(name);
			}
		}

		Model::Model(const std::string& file) : filePath(file)
		{
			resource = Application::getCache()->emplace<ModelResource>(file);
		}

		SkinnedMeshRenderer::SkinnedMeshRenderer(const std::shared_ptr<Mesh>& meshes) : mesh(meshes)
		{
		}
		auto SkinnedMeshRenderer::isActive() const -> bool
		{
			return mesh ? mesh->isActive() : false;
		}
		auto SkinnedMeshRenderer::getMesh(const std::string& name) -> void
		{
			auto currentScene = Application::get()->getSceneManager()->getCurrentScene();
			Entity ent{ entity, currentScene->getRegistry() };
			auto model = ent.tryGetComponentFromParent<Model>();
			mesh = model->resource->find(name);
		}
	};
};