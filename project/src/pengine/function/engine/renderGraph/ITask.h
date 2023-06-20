#pragma once
#include "core/core.h"
#include "IRenderGraphResource.h"
#include <vector>
#include <memory>
#include "function/ecs/Entity/Entity.h"
#include "entt/entt.hpp"
namespace pengine
{
	class RenderGraph;
	struct SharedRenderData;
	class ITask
	{
	public:
		ITask(uint32_t	_uid, RenderGraph* renderGraph) : uid(_uid), m_renderGraph(renderGraph){};
	public:
		virtual auto init(entt::registry& registry) -> void = 0;
		virtual auto execute(CommandBuffer* commandBuffer) -> void = 0;
		virtual auto setup() -> void = 0;
		virtual auto onUpdate(entt::registry& registry) -> void = 0;
		virtual auto onResize(uint32_t width, uint32_t height, uint32_t displayWidth, uint32_t displayHeight) -> void = 0;
		virtual auto onSceneElementChange(Entity& ent) -> void = 0;
	public:
		//don't do any validation here
		//don't get any task info between task
		auto bindInput(size_t index, std::shared_ptr<RenderGraphVirtualResource> p_vRes) -> void
		{
			PENGINE_ASSERT(index<inputs.size(), "Fatal : inputs index overflow when binding task input" );
			inputs.at(index) = p_vRes;
			inputs.at(index)->b_initialized = true;
		};
		inline auto getInputType(size_t index) -> RenderResouceType
		{
			PENGINE_ASSERT(index < inputs.size(), "Fatal : inputs index overflow when get task input");
			return inputs.at(index)->type;
		};
		inline auto getOutput(size_t index) -> std::shared_ptr<RenderGraphVirtualResource>
		{
			PENGINE_ASSERT(index < inputs.size(), "Fatal : inputs index overflow when get task out");
			return outputs.at(index);
		}
		inline auto getOutputType(size_t index) -> RenderResouceType
		{
			return getOutput(index)->type;
		};
		inline auto getInputCount() -> size_t { return inputs.size(); };
		inline auto getOutputCount() -> size_t { return outputs.size(); };
		inline auto getState() -> bool { return b_completed; };
		inline auto setDependency(uint32_t dp) -> void 
		{
			if (std::find(dependencies.begin(), dependencies.end(), dp) == dependencies.end())
			{
				dependencies.push_back(dp);
			}
		};
	protected:
		//name
		char* name;
		//reads
		std::vector <std::shared_ptr<RenderGraphVirtualResource>> inputs;
		//writes
		std::vector <std::shared_ptr<RenderGraphVirtualResource>> outputs;
		//unique id (unique id in graph)
		//it should be allocated in the config file. insure it's unique
		uint32_t uid;
		//hold the visible entity by each task
		//avoid update all every frame
		std::vector<Entity> m_visbileEntity;

		//record dependencies
		std::vector<uint32_t> dependencies;

		bool b_completed = false;
		RenderGraph* m_renderGraph;
	};


};