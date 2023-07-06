#include "SystemManager.h"
namespace pengine
{

	auto SystemManager::onInit() -> void
	{
		for (auto& system : systems)
			system.second->onInit();
	}
	auto SystemManager::onUpdate(float dt, Scene* scene) -> void
	{
		for (auto& system : systems)
			system.second->onUpdate(dt, scene);
	}

	auto SystemManager::release() -> void
	{
		for (auto& system : systems)
			system.second->release();
	}

	auto SystemManager::onImGui()-> void
	{
		for (auto& system : systems)
			system.second->onImGui();
	}
};