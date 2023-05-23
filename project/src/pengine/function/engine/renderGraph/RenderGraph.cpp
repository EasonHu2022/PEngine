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
		//bind tasks
		//bind();
		//bind();
		//bind();
		
	}
	auto RenderGraph::compile() -> void
	{
		//bind tasks
		//bind();
		//bind();
		//bind();
	}

	auto RenderGraph::execute() -> void
	{
	}

	auto RenderGraph::bind(uint32_t outputTask, size_t OutputBindPos, uint32_t inputTask, size_t inputBindPos) -> bool
	{
		if (taskMap.find(outputTask) != taskMap.end()) {
			
			auto p_vRes = taskMap[outputTask]->getOutput(OutputBindPos);
			if (taskMap.find(inputTask) != taskMap.end())
			{
				taskMap[inputTask]->bindInput(inputBindPos, p_vRes);
				switch (p_vRes->type)
				{
				case RenderResouceType::Res_Texture2D:
					//resources.emplace_back(std::make_shared<RenderGraphTexture2DResource>())->create(width,height,commandBuffer, p_vRes->name);
					break;
				default:
					break;
				}
			}
			else
			{
				PLOGE("try to bind a nonexistent input task , uid: {0}", inputTask);
			}
		}
		else {
			PLOGE("try to bind a nonexistent output task , uid: {0}", outputTask);
		}
		
		
		
	}
};