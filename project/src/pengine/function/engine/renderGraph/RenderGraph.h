#pragma once
#include <memory>
#include <vector>
#include <unordered_map>
#include <entt/entity/fwd.hpp>
#include "IPass.h"
#include "IRenderGraphResource.h"
#include "function/render/rhi/CommandBuffer.h"
#include "function/engine/Material.h"
#include "glm/glm.hpp"
#include "function/engine/Mesh.h"
#include "entt/entt.hpp"

namespace pengine
{

	class PassGroup;
	class RenderGraph
	{
	public:
		RenderGraph(std::string& path);
		~RenderGraph() {};
		auto init(entt::registry& registry,uint32_t width, uint32_t height, uint32_t displayWidth, uint32_t displayHeight) -> void;
		auto setup() -> void;
		//update cpu data for render
		auto update(entt::registry& registry) -> void;
		auto compile() -> void;
		auto execute(CommandBuffer* cmdBuffer) -> void;
		auto bind(uint32_t outputTask, size_t bindpos, uint32_t inputTask, size_t bindPos) -> bool;
		auto onResize(uint32_t width, uint32_t height, uint32_t displayWidth, uint32_t displayHeight) -> void;
		auto getResourceByID(uint32_t id) -> IRenderGraphResource*;

		auto inline getPassByID(uint32_t id) ->std::shared_ptr<IPass> { return passMap[id]; };
		auto inline getPassCount()->size_t { return passUids.size(); };
	private:
		//Topological Sorting
		auto passSorting(std::vector<uint32_t> src) -> void;
	private:

		
		template<class T>
		auto addPass(uint32_t uid) -> std::shared_ptr<T>
		{
			static_assert(std::is_base_of<ITask, T>::value, "class T should extend from ITask");
			PLOGI("Add Task: {0}", typeid(T).name());
			passUids.push_back(uid);
			return std::static_pointer_cast<T>(passMap.emplace(uid, std::make_shared<T>(uid,this)).first->second);
		}
		std::string name;

		std::vector<uint32_t> passUids;
		std::unordered_map<uint32_t,std::shared_ptr<IPass>> passMap;
		std::vector<std::shared_ptr<IRenderGraphResource>> resources;
		std::string path;
		glm::vec2 renderExtend;
		glm::vec2 displayExtend;
		//set of group
		std::vector<PassGroup> groupSet;
	};

};