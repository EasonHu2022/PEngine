#pragma once
#include "core/core.h"
#include "IRenderGraphResource.h"
#include <vector>
#include <memory>
namespace pengine
{
	class ITask
	{
	public:
		ITask(uint32_t	_uid) : uid(_uid) {};
	public:
		virtual auto init() -> void = 0;
		virtual auto execute() -> void = 0;
		virtual auto setup() -> void = 0;
	public:
		//don't do any validation here
		//don't get any task info between task
		auto bindInput(size_t index, std::shared_ptr<RenderGraphVirtualResource> p_vRes) -> void
		{
			PENGINE_ASSERT(index<inputs.size(), "Fatal : inputs index overflow when binding task input" );
			inputs.at(index) = p_vRes;
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
		
	private:
		char* name;
		//reads
		std::vector <std::shared_ptr<RenderGraphVirtualResource>> inputs;
		//writes
		std::vector <std::shared_ptr<RenderGraphVirtualResource>> outputs;
		//unique id (unique id in graph)
		//it should be allocated in the config file. insure it's unique
		uint32_t uid;
	};


};