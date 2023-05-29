#pragma once
#include <unordered_map>
#include <vector>
#include <memory>
#include <Queue>
#include "function/ecs/Entity/Entity.h"
#include "RenderQueue.h"
namespace pengine
{
	class Scene
	{
	public:
		Scene();
		~Scene();
		auto cull() -> void;
		auto setUp() -> void;
		auto onUpdate() -> void;
		//return a handle , 0 -> invalid
		auto addEntity() -> size_t;
		auto removeEntity(size_t handle) -> bool;

	private:
		auto updateRenderQueue() -> void;
		std::unordered_map<size_t,RenderQueue> queueMap;
		std::vector<std::shared_ptr<Entity>> entitys;
	};
}