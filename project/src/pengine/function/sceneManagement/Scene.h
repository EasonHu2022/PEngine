#pragma once
#include <unordered_map>
#include <vector>
#include <memory>
#include <Queue>
#include <string>
#include "function/ecs/Entity/Entity.h"
#include "function/ecs/Entity/EntityManager.h"
#include "function/ecs/Component/Light.h"
#include "function/ecs/Component/Camera.h"
#include "function/ecs/component/Transform.h"
#include "function/resource/IResource.h"
#include "function/engine/renderGraph/RenderGraph.h"
namespace pengine
{

	class PENGINE_API Scene : public IResource
	{
	public:
		Scene(const std::string& name, uint32_t width, uint32_t height, std::shared_ptr<Texture> renderTarget);
		Scene(const std::string &name);
		~Scene();
		auto getResourceType() const->FileType override { return FileType::Scene; };
		auto getPath() const->std::string override { return path; };
		auto setPath(std::string _path) ->void { path = _path; };
		auto cull() -> void;
		auto onUpdate() -> void;
		auto onLateUpdate() -> void;
		auto onRender() -> void;
		auto onResize(size_t width, size_t height) -> void;
		auto getRegistry() -> entt::registry&;
		auto createEntity() -> Entity;
		auto createEntity(const std::string& name) -> Entity;
		auto duplicateEntity(const Entity& entity, const Entity& parent) -> void;
		auto duplicateEntity(const Entity& entity) -> void;
		auto removeAllChildren(entt::entity entity) -> void;
		auto getCamera() -> std::pair<component::Camera*,component::Transform*>;
		auto addModel(const std::string filePath) -> Entity;

		inline auto& getEntityManager()
		{
			return entityManager;
		}
	protected:
		std::shared_ptr<EntityManager> entityManager;
		std::string                    name;
		std::string                    path;
		std::function<void(Entity)> onEntityAdd;
		bool     dirty = false;
		
		std::shared_ptr <RenderGraph> renderGraph;

		uint32_t width = 0;
		uint32_t height = 0;
	};
}