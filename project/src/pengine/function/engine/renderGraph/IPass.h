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
	class IPass
	{
	public:
		IPass(uint32_t	_uid, RenderGraph* renderGraph) : uid(_uid), m_renderGraph(renderGraph){};
	public:
		virtual auto init(entt::registry& registry) -> void = 0;
		virtual auto execute(CommandBuffer* commandBuffer) -> void = 0;
		virtual auto setup() -> void = 0;
		virtual auto onUpdate(entt::registry& registry) -> void = 0;
		virtual auto onResize(uint32_t width, uint32_t height, uint32_t displayWidth, uint32_t displayHeight) -> void = 0;
	public:
		//don't do any validation here
		//don't get any task info between task
		auto bindInput(size_t index, std::shared_ptr<RenderGraphVirtualResource> p_vRes) -> void
		{
			PENGINE_ASSERT(index<inputs.size(), "Fatal : inputs index overflow when binding task input" );
			inputs.at(index) = p_vRes;
		};
		//when process WAW dependencies
		auto bindOutput(size_t index, std::shared_ptr<RenderGraphVirtualResource> p_vRes) -> void
		{
			PENGINE_ASSERT(index < outputs.size(), "Fatal : inputs index overflow when binding task input");
			outputs.at(index) = p_vRes;
		};

		inline auto getInputType(size_t index) -> RenderResouceType
		{
			PENGINE_ASSERT(index < inputs.size(), "Fatal : inputs index overflow when get task input");
			return inputs.at(index)->type;
		};
		inline auto getOutput(size_t index) -> std::shared_ptr<RenderGraphVirtualResource>
		{
			PENGINE_ASSERT(index < outputs.size(), "Fatal : outputs index overflow when get task out");
			return outputs.at(index);
		}
		inline auto getOutputType(size_t index) -> RenderResouceType
		{
			return getOutput(index)->type;
		};
		inline auto getInputCount() -> size_t { return inputs.size(); };
		inline auto getOutputCount() -> size_t { return outputs.size(); };
		inline auto addDegreeIn(uint32_t dp) -> void 
		{
			if (std::find(m_InPasses.begin(), m_InPasses.end(), dp) == m_InPasses.end())
			{
				m_InPasses.push_back(dp);
				m_inDegree = m_InPasses.size();
			}
		};

		inline auto updateDegree(uint32_t dp) -> void
		{
			if (std::find(m_InPasses.begin(), m_InPasses.end(), dp) != m_InPasses.end())
			{
				m_inDegree--;
			}
		}

		inline auto getDegree() -> size_t
		{
			return m_inDegree;
		}

		inline auto getUID() -> uint32_t
		{
			return uid;
		}

	protected:
		//name
		char* name;
		//reads(dependencies res)
		std::vector <std::shared_ptr<RenderGraphVirtualResource>> inputs;
		//writes
		std::vector <std::shared_ptr<RenderGraphVirtualResource>> outputs;
		//unique id (unique id in graph)
		//it should be allocated in the config file. insure it's unique
		uint32_t uid;
		
		//record inpasses
		std::vector<uint32_t> m_InPasses;

		//only used for order
		size_t m_inDegree = 0;

		RenderGraph* m_renderGraph;
	};


};