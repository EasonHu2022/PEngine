#include "OutputPass.h"
#include "RenderGraph.h"
#include "function/render/rhi/RenderDevice.h"
#include "function/render/rhi/Shader.h"
#include "function/render/rhi/Pipeline.h"
#include "Application.h"
namespace pengine
{
	OutputPass::OutputPass(uint32_t _uid, RenderGraph* renderGraph) : IPass(_uid, renderGraph)
	{
	}
	OutputPass::~OutputPass()
	{
	}
	auto OutputPass::init(entt::registry& registry) -> void 
	{
		createVResource();
		m_outputPassData = OutputPassData();
		//simply do nothing
	}
	//copy the former output to the render target texture provided outside
	auto OutputPass::execute(CommandBuffer* commandBuffer) -> void 
	{
		auto outTexture = m_renderGraph->getOutputTexture();
		if (outTexture == nullptr)
		{
			PLOGW("none texture target is set to the current applied rendergraph");
			return;
		}
		//tmply
		//RenderDevice::copyImage(commandBuffer,m_renderGraph->getResourceByID(inputs[0]->index)->getNativeResource().get(), texture);
		//todo-do a screen shader do nothing just sampling
		auto color = m_renderGraph->getResourceByID(inputs[0]->index)->getNativeResource();
		m_outputPassData.descriptorOutputSet[0]->setTexture("outputColor", color);
		m_outputPassData.descriptorOutputSet[0]->update();
		PipelineInfo pipeInfo;
		pipeInfo.shader = m_outputPassData.outputShader;
		pipeInfo.polygonMode = PolygonMode::Fill;
		pipeInfo.cullMode = CullMode::None;
		pipeInfo.transparencyEnabled = false;
		pipeInfo.depthBiasEnabled = false;
		pipeInfo.clearTargets = false;
		//output to outside
		pipeInfo.colorTargets[0] = outTexture;
		auto pipeline = Pipeline::get(pipeInfo);
		pipeline->bind(commandBuffer);
		RenderDevice::bindDescriptorSets(pipeline.get(), commandBuffer, 0, m_outputPassData.descriptorOutputSet);
		RenderDevice::drawMesh(commandBuffer, pipeline.get(), m_outputPassData.screenQuad.get());
		pipeline->end(commandBuffer);
	}
	auto OutputPass::setup() -> void 
	{
		//simply do nothing
	}
	auto OutputPass::onUpdate(entt::registry& registry) -> void 
	{
		//simply do nothing
	}
	auto OutputPass::onResize(uint32_t width, uint32_t height, uint32_t displayWidth, uint32_t displayHeight) -> void
	{
		//simply do nothing
	}
	auto OutputPass::createVResource() -> void
	{
		inputs.resize(1); 
		for (int i = 0; i < inputs.size(); i++)
		{
			auto res = new RenderGraphVirtualResource();
			res->type = RenderResouceType::Res_Texture2D;
			res->format = TextureFormat::RGBA16;
			auto renderExtend = m_renderGraph->getRenderExtend();
			res->width = renderExtend.x;
			res->height = renderExtend.y;
			inputs[i] = std::shared_ptr<RenderGraphVirtualResource>(res);
		}
	}
	OutputPassData::OutputPassData()
	{
		std::string tmpPath = "shaders/FinalOutput.shader";
		outputShader = Shader::create(ASSETS_ABSOLUTE_PATH + tmpPath);
		DescriptorInfo info{};
		descriptorOutputSet.resize(1);
		info.shader = outputShader.get();
		info.layoutIndex = 0;
		descriptorOutputSet[0] = DescriptorSet::create(info);
		screenQuad = Mesh::createQuad(true);
	}
};