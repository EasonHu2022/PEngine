#pragma once
#include "ITask.h"
#include "IRenderGraphResource.h"
#include <memory>
#include <unordered_map>
#include "function/render/rhi/CommandBuffer.h"
#include "glm/glm.hpp"
namespace pengine
{
	struct CommonRenderData
	{
		

	};


	class RenderGraph
	{
	public:
		RenderGraph(CommonRenderData* data);
		~RenderGraph() {};
		auto init(uint32_t width, uint32_t height) -> void;
		auto setup() -> void;
		auto compile() -> void;
		auto execute() -> void;
		auto bind(uint32_t outputTask, size_t bindpos, uint32_t inputTask, size_t bindPos) -> bool;
		auto getTaskCount() -> size_t;
		
	private:
		template<class T>
		auto addTask(uint32_t uid) -> std::shared_ptr<T>
		{
			static_assert(std::is_base_of<ITask, T>::value, "class T should extend from ITask");
			PLOGI("Add Task: {0}", typeid(T).name());
			taskUids.push_back(uid);
			return std::static_pointer_cast<T>(taskMap.emplace(uid, std::make_shared<T>(uid)).first->second);
		}
		auto createCBuffers() -> void;
		std::vector<uint32_t> taskUids;
		std::unordered_map<uint32_t,std::shared_ptr<ITask>> taskMap;
		std::vector<std::shared_ptr<IRenderGraphResource>> resources;
		//maybe queue?
		//std::vector<renderQueue> queues;//make it as vertex input
	};

};