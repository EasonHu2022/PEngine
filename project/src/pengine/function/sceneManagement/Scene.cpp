#include "Scene.h"
#include "Application.h"
#include "function/render/rhi/SwapChain.h"
#include "core/utils/StringUtils.h"
#include "function/ecs/component/MeshRenderer.h"
#include "function/engine/Skeleton.h"
#include "function/ecs/component/Transform.h"
#include "function/render/rhi/Texture.h"
#include "SceneGraph.h"
namespace pengine
{
	namespace
	{
		inline auto addEntity(Scene* scene, Entity parent, Skeleton* skeleton, const Bone& bone) -> void
		{
			auto entity = scene->createEntity(bone.name);
			auto& transform = entity.addComponent<component::Transform>();
			transform.setLocalTransform(bone.offsetMatrix);
			entity.setParent(parent);

			for (auto child : bone.children)
			{
				addEntity(scene, entity, skeleton, skeleton->getBones()[child]);
			}
		}
	}

	Scene::Scene(const std::string& name, uint32_t _width, uint32_t _height, std::shared_ptr<Texture> renderTarget)
	{
		entityManager = std::make_shared<EntityManager>();
		sceneGraph = std::make_shared<SceneGraph>(entityManager);
		//each scene bind with a render graph
		std::string RGPath = "test.rg";
		renderGraph = std::make_shared<RenderGraph>(RGPath);
		width = _width;
		height = _height;
		renderGraph->init(entityManager->getRegistry(), width, height, width, height,sceneGraph);
		renderGraph->setOutputTexture(renderTarget);
	}

	Scene::Scene(const std::string& _name) : name(_name)
	{
		entityManager = std::make_shared<EntityManager>();
		sceneGraph = std::make_shared<SceneGraph>(entityManager);
		//each scene bind with a render graph
		std::string RGPath = "test.rg";
		renderGraph = std::make_shared<RenderGraph>(RGPath);
		//temply equal to swapchain extend
		width = Application::getWindow()->getWidth();
		height = Application::getWindow()->getHeight();
		renderGraph->init(entityManager->getRegistry(),width, height,width,height, sceneGraph);
	}
	Scene::~Scene()
	{
		renderGraph.reset();
	}
	//cull camera
	auto Scene::cull() -> void
	{
		auto [camera, transform] = getCamera();
		auto& frustum = camera->getFrustum(glm::inverse(transform->getWorldMatrix()));
		culledEntities.clear();
		sceneGraph->frustumCull(frustum,culledEntities);
	}
	auto Scene::onUpdate() -> void
	{
		//update for ui/objects/ or sth
		//set world matrix (parent)
		//current non parent so just set I.
		auto views = getRegistry().view<component::Transform>(entt::exclude<component::Hierarchy>);
		for (auto ent : views)
		{
			getRegistry().get<component::Transform>(ent).setWorldMatrix(glm::mat4{ 1.f });
		}
		

	}
	auto Scene::onLateUpdate() -> void
	{
		//do cull
		cull();
		//very last order
		renderGraph->update(entityManager->getRegistry(),culledEntities);
	}

	auto Scene::onRender() -> void
	{
		renderGraph->execute(Application::getGraphicsContext()->getSwapChain()->getCurrentCommandBuffer());
	}

	auto Scene::onResize(size_t width, size_t height) -> void
	{
		Application::getGraphicsContext()->forceClearImmediately();
		Application::getGraphicsContext()->forceFlushDeletionQueue();
		//resize renderGraph
		auto [camera,transform] = getCamera();
		camera->setAspectRatio(static_cast<float>(width)/height);
		renderGraph->onResize(width,height,width,height);
		Application::getGraphicsContext()->delayFlushDeletionQueue();
	}

	auto Scene::getRegistry() -> entt::registry&
	{
		return entityManager->getRegistry();
	}

	auto Scene::createEntity() -> Entity
	{
		dirty = true;
		auto entity = entityManager->create();
		if (onEntityAdd)
			onEntityAdd(entity);
		return entity;
	}

	auto Scene::createEntity(const std::string& name) -> Entity
	{
		dirty = true;
		int32_t i = 0;
		auto    entity = entityManager->getEntityByName(name);
		while (entity.valid())
		{
			entity = entityManager->getEntityByName(name + "(" + std::to_string(i + 1) + ")");
			i++;
		}
		auto newEntity = entityManager->create(i == 0 ? name : name + "(" + std::to_string(i) + ")");
		if (onEntityAdd)
			onEntityAdd(newEntity);
		return newEntity;
	}

	auto Scene::duplicateEntity(const Entity& entity, const Entity& parent) -> void
	{
		
	}

	auto Scene::duplicateEntity(const Entity& entity) -> void
	{
	}

	auto Scene::removeAllChildren(entt::entity entity) -> void
	{
		entityManager->removeAllChildren(entity);
	}

	auto Scene::getCamera() -> std::pair<component::Camera*, component::Transform*>
	{
		auto cameraView = getRegistry().group<component::Camera>(entt::get<component::Transform>);
		if (!cameraView.empty())
		{
			Entity     entity(cameraView.front(), getRegistry());
			component::Camera& sceneCam = entity.getComponent<component::Camera>();
			component::Transform& sceneCamTr = entity.getComponent<component::Transform>();
			return { &sceneCam, &sceneCamTr };
		}
		else
		{
			return { nullptr,nullptr };
		}
	}

	auto Scene::addModel(const std::string filePath) -> Entity
	{
		auto  name = StringUtils::getFileNameWithoutExtension(filePath);
		auto modelEntity = createEntity(name);
		auto& model = modelEntity.addComponent<component::Model>(filePath);
		//if single mesh
		if (model.resource->getMeshes().size() == 1)
		{
			modelEntity.addComponent<component::MeshRenderer>(model.resource->getMeshes().begin()->second);
		}
		else
		{
			for (auto& mesh : model.resource->getMeshes())
			{
				auto child = createEntity(mesh.first);
				if (model.resource->getSkeleton())
				{
					child.addComponent<component::SkinnedMeshRenderer>(mesh.second);
				}
				else
				{
					child.addComponent<component::MeshRenderer>(mesh.second);

				}
				child.setParent(modelEntity);
			}
			auto skeleton = model.resource->getSkeleton();
			if (skeleton)
			{
				skeleton->buildRoot();
				for (auto child : skeleton->getRoots())
				{
					addEntity(this, modelEntity, skeleton.get(), skeleton->getBones()[child]);
				}
			}
		}
		model.type = component::PrimitiveType::File;
		//tmply build here
		sceneGraph->build();
		return modelEntity;
	}

	auto Scene::release() -> void
	{
	}
	
};