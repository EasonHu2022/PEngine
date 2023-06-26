#include "Scene.h"
#include "function/engine/renderGraph/RenderGraph.h"
#include "Application.h"
#include "function/render/rhi/SwapChain.h"
namespace pengine
{
	Scene::Scene(const std::string& _name) : name(_name)
	{
		//each scene bind with a render graph
		renderGraph = std::make_shared<RenderGraph>("");
		renderGraph->init(entityManager->getRegistry(),width, height,width,height);
	}
	Scene::~Scene()
	{
	}
	auto Scene::cull() -> void
	{
	}
	auto Scene::onUpdate() -> void
	{


		
	}
	auto Scene::onLateUpdate() -> void
	{
		//very last order
		renderGraph->update(entityManager->getRegistry());
	}

	auto Scene::onRender() -> void
	{
		renderGraph->execute(Application::getGraphicsContext()->getSwapChain()->getCurrentCommandBuffer());
	}

	auto Scene::onResize(size_t width, size_t height) -> void
	{
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
	
};