#include "RenderGraph.h"
#include "RenderGBufferPass.h"
#include "RenderDeferredLightingPass.h"
#include "OutputPass.h"
#include "RenderShadowMapPass.h"
#include "RenderGraphTexture2DResource.h"
#include "RenderGraphDepthTextureArrayResource.h"
#include "function/ecs/component/Component.h"
#include "function/ecs/component/Transform.h"
#include "function/ecs/component/MeshRenderer.h"
#include "function/ecs/component/camera.h"
#include "PassGroup.h"
#include "function/render/rhi/GraphicsContext.h"
#include "Application.h"
namespace pengine
{
	RenderGraph::RenderGraph(std::string& _path) : path(_path)
	{
	}
	RenderGraph::~RenderGraph()
	{
		//release passes and resources
		resources.clear();
		passMap.clear();


	}
	auto RenderGraph::init(entt::registry& registry, uint32_t width, uint32_t height, uint32_t displayWidth, uint32_t displayHeight) -> void
	{
		renderExtend = { width ,height };
		outputExtend = { displayWidth ,displayHeight };
		//TODO:init renderData//how?what structure?
		//renderDatacontext...
		//temply write to hard code, later load from config file
		/*################# Add Tasks #######################*/
		//add with uid, read from config 
		addPass<RenderGBufferPass>(1) -> init(registry);
		addPass<RenderDeferredLightingPass>(2) -> init(registry);
		addPass<OutputPass>(3)->init(registry);
		addPass<RenderShadowMapPass>(4)->init(registry);
		/*################# */
		compile();
	}
	auto RenderGraph::setup() -> void
	{
	
		
		

	}
	//sink registry, get data each layer
	auto RenderGraph::update(entt::registry& registry, std::vector<entt::entity>& culledEnts) -> void
	{
		auto taskCount = passUids.size();
		for (int i = 0; i < taskCount; i++)
		{
			passMap[passUids.at(i)]->onUpdate(registry, culledEnts);
		}
	}

	auto RenderGraph::compile() -> void
	{
		compileDependency();
		createResourceMap();

		//sort passes
		passSorting(passUids);
		//compile each pass group
		for (auto& group : groupSet)
		{
			group.compile();
		}
	}
	auto RenderGraph::execute(CommandBuffer* cmdBuffer) -> void
	{
		//execute each pass group
		for (auto& group : groupSet)
		{
			group.execute(cmdBuffer);
		}
		
	}
	auto RenderGraph::bindInput(uint32_t outputTask, size_t OutputBindPos, uint32_t inputTask, size_t inputBindPos) -> bool
	{
		if (passMap.find(outputTask) != passMap.end()) {
			
			auto p_vRes = passMap[outputTask]->getOutput(OutputBindPos);
			if (passMap.find(inputTask) != passMap.end())
			{
				auto outputType = passMap[outputTask]->getOutputType(OutputBindPos);
				auto inputType = passMap[inputTask]->getInputType(inputBindPos);
				if (outputType != inputType)
				{
					PLOGE("Failed! Try to bind 2 resource with different type ! out : {0}, in : {1}", outputType,inputType);
					return false;
				}

				passMap[inputTask]->bindInput(inputBindPos, p_vRes);
				//set dependencies task(used to Topological Sorting)
				passMap[inputTask]->addDegreeIn(outputTask);
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
	auto RenderGraph::bindOutput(uint32_t prevPass, size_t prevbindpos, uint32_t postPass, size_t postbindPos) -> bool
	{
		if (passMap.find(prevPass) != passMap.end()) {

			auto p_vRes = passMap[prevPass]->getOutput(prevbindpos);
			if (passMap.find(postPass) != passMap.end())
			{
				if (passMap[prevPass]->getOutputType(prevbindpos) != passMap[postPass]->getInputType(postbindPos))
				{
					PLOGE("Failed! Try to bind 2 resource with different type !");
					return false;
				}

				passMap[postPass]->bindOutput(postbindPos, p_vRes);
				//set dependencies task(used to Topological Sorting)
				passMap[postPass]->addDegreeIn(prevPass);			
			}
			else
			{
				PLOGE("try to bind a nonexistent output pass , uid: {0}", postPass);
				return false;
			}
		}
		else {
			PLOGE("try to bind a nonexistent output task , uid: {0}", prevPass);
			return false;
		}
		return true;
	}
	auto RenderGraph::onResize(uint32_t width, uint32_t height, uint32_t displayWidth, uint32_t displayHeight) -> void
	{
		renderExtend = { width ,height };
		outputExtend = { displayWidth ,displayHeight };
		//release and recreate resources
		auto passCount = passMap.size();
		for (int i = 0; i < passCount; i++)
		{
			passMap[passUids.at(i)]->onResize(width,height,displayWidth,displayHeight);
		}
		for (auto &r : resources)
		{
			auto i = r.use_count();
			PLOGE("use count : {0}",i);
		}
		resources.clear();
		
		createResourceMap();
	}
	auto RenderGraph::getResourceByID(uint32_t id) -> std::shared_ptr<IRenderGraphResource>
	{
		if (id >= resources.size())
		{
			PLOGE("try to get resource id : {0}£¬ but only {1} resources in the map ! ",id,resources.size());
			return NULL;
		}
		return resources.at(id);
	}
	auto RenderGraph::compileDependency() -> void
	{
		//-- bindoutput -- WAW

		//-- bindintout -- RAW
		bindInput(1, 0, 2, 0);
		bindInput(1, 1, 2, 1);
		bindInput(1, 2, 2, 2);
		bindInput(1, 3, 2, 3);
		bindInput(2, 0, 3, 0);
		bindInput(4, 0, 2, 4);
	}
	auto RenderGraph::createResourceMap() -> void
	{
		for (auto pass : passMap)
		{
			auto n = pass.second->getOutputCount();
			for (int i = 0; i < n; i++)
			{
				auto p_vRes = pass.second->getOutput(i);
				switch (p_vRes->type)
				{
				case RenderResouceType::Res_Texture2D:
					if (!p_vRes->b_initialized)
					{
						p_vRes->index = resources.size();
						resources.emplace_back(std::make_shared<RenderGraphTexture2DResource>())->create(p_vRes->width, p_vRes->height,
							nullptr, p_vRes->name, p_vRes->format);
						p_vRes->b_initialized = true;
					}
					break;
				case RenderResouceType::Res_DepthArray:
					if (!p_vRes->b_initialized)
					{
						p_vRes->index = resources.size();
						resources.emplace_back(std::make_shared<RenderGraphDepthTextureArrayResource>())->create(p_vRes->width, p_vRes->height,
							nullptr, p_vRes->name, p_vRes->format, p_vRes->layer);
						p_vRes->b_initialized = true;
					}
					break;
				default:
					break;
				}
			}
		}
		
	}
	auto RenderGraph::passSorting(std::vector<uint32_t> src) -> void
	{
		auto count = src.size();
		if (count == 0) return;
		PassGroup group_ordered(this);
		std::vector<uint32_t> unordered;
		for (int i = 0; i < count; i++)
		{
			auto pass = passMap[src[i]];
			if (pass->getDegree() == 0)
			{
				group_ordered.addPass(pass->getUID());
			}
			else
			{
				unordered.push_back(pass->getUID());
			}
		}
		auto size = group_ordered.getPassesCount();
		for (int i = 0; i < size; i++)
		{
			for (int j = 0; j < count - size; j++)
			{
				auto pass = passMap[unordered[j]];
				pass->updateDegree(group_ordered.getPass(i));
			}
		}
		groupSet.push_back(group_ordered);
		passSorting(unordered);
	}
};