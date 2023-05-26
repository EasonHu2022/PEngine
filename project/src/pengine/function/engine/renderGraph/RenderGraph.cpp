#include "RenderGraph.h"
#include "RenderGBufferTask.h"
#include "RenderDeferredLightingTask.h"
#include "RenderGraphTexture2DResource.h"
namespace pengine
{
	auto RenderGraph::init(uint32_t width, uint32_t height) -> void
	{
		
		//TODO:init renderData//how?what structure?
		//renderDatacontext...
		//temply write to hard code, later load from config file
		/*################# Add Tasks #######################*/
		//add with uid, read from config 
		addTask<RenderGBufferTask>(1) -> init();
		addTask<RenderDeferredLightingTask>(2) -> init();
		/*################# */
	}
	auto RenderGraph::setup() -> void
	{
		//commit CBuffer

	}
	auto RenderGraph::compile() -> void
	{
		//bind tasks and create real resources
		bind(1, 0, 2, 0);
		bind(1, 1, 2, 1);
		bind(1, 2, 2, 2);
		bind(1, 3, 2, 3);
		bind(1, 4, 2, 4);
	}

	auto RenderGraph::createCBuffers() ->void
	{

	}

	auto RenderGraph::execute() -> void
	{
		//execute each task
		auto taskCount = taskUids.size();
		for (int i = 0; i < taskCount; i++)
		{
			taskMap[taskUids.at(i)]->execute();
		}
	}

	auto RenderGraph::bind(uint32_t outputTask, size_t OutputBindPos, uint32_t inputTask, size_t inputBindPos) -> bool
	{
		if (taskMap.find(outputTask) != taskMap.end()) {
			
			auto p_vRes = taskMap[outputTask]->getOutput(OutputBindPos);
			if (taskMap.find(inputTask) != taskMap.end())
			{
				if (taskMap[outputTask]->getOutputType(OutputBindPos) != taskMap[inputTask]->getInputType(inputBindPos))
				{
					PLOGE("Failed! Try to bind 2 resource with different type !");
					return false;
				}
				taskMap[inputTask]->bindInput(inputBindPos, p_vRes);
				switch (p_vRes->type)
				{
				case RenderResouceType::Res_Texture2D:
					if (!p_vRes->b_initialized)
					{
						p_vRes->b_initialized = true;
						p_vRes->index = resources.size();
						resources.emplace_back(std::make_shared<RenderGraphTexture2DResource>())->create(p_vRes->width, p_vRes->height,
							commandBuffer, p_vRes->name, p_vRes->format);
					}		
					break;
				default:
					break;
				}
			}
			else
			{
				PLOGE("try to bind a nonexistent input task , uid: {0}", inputTask);
				return false;
			}
		}
		else {
			PLOGE("try to bind a nonexistent output task , uid: {0}", outputTask);
			return false;
		}
		return true;
	}
};