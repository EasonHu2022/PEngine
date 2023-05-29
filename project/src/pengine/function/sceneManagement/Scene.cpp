#include "Scene.h"
namespace pengine
{
	Scene::Scene()
	{
	}
	Scene::~Scene()
	{
	}
	auto Scene::cull() -> void
	{
	}
	auto Scene::setUp() -> void
	{
	}
	auto Scene::onUpdate() -> void
	{
	}
	auto Scene::addEntity() -> size_t
	{
		return size_t();
	}
	auto Scene::removeEntity(size_t handle) -> bool
	{
		return false;
	}
	auto Scene::updateRenderQueue() -> void
	{
	}
};