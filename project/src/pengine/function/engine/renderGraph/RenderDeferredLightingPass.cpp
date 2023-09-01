#include "RenderDeferredLightingPass.h"
#include "RenderGraph.h"
#include "function/render/rhi/DescriptorSet.h"
#include "function/render/rhi/Pipeline.h"
#include "function/render/rhi/RenderDevice.h"
#include "function/render/rhi/Shader.h"
#include "function/ecs/component/Light.h"
#include "function/ecs/component/Transform.h"
#include "function/ecs/component/Camera.h"
#include "glm/glm.hpp"
#include <Application.h>
#include "function/profile/profiler.h"
#include "function/render/rhi/Texture.h"
namespace pengine
{
	RenderDeferredLightingPass::RenderDeferredLightingPass(uint32_t uid, RenderGraph* renderGraph) : IPass(uid, renderGraph)
	{
		name = "Deferred Lighting Pass";
	}
	RenderDeferredLightingPass::~RenderDeferredLightingPass()
	{
	}
	auto RenderDeferredLightingPass::init(entt::registry& registry) -> void
	{
		createVResource();
		m_RenderDefferredLightingData = RenderDeferredLightingData();
	}
	auto RenderDeferredLightingPass::execute(CommandBuffer* commandBuffer) -> void
	{
		auto descriptorSet = m_RenderDefferredLightingData.descriptorLightSet[0];
		//common data part
		auto& commonData = m_renderGraph->getCommonData();
		descriptorSet->setUniform("UniformBufferLight", "shadowTransform", commonData.shadowTransforms);
		descriptorSet->setUniform("UniformBufferLight", "splitDepths", commonData.splitDepth);
		descriptorSet->setUniform("UniformBufferLight", "shadowMapSize", &commonData.shadowMapSize);
		descriptorSet->setUniform("UniformBufferLight", "shadowCount", &commonData.shadowMapNum);
		descriptorSet->setUniform("UniformBufferLight", "biasMatrix", &commonData.biasMatrix);
		descriptorSet->setUniform("UniformBufferLight", "inverseVP", &commonData.inverseCameraVP);
		auto color = m_renderGraph->getResourceByID(inputs[(int)DeferredLightingInput::COLOR]->index)->getNativeResource();
		//PLOGE("color first excute pre : {0}",color.use_count());
		descriptorSet->setTexture(InputName[(int)DeferredLightingInput::COLOR], color);
		//PLOGE("color first excute post : {0}", color.use_count());
		auto positon = m_renderGraph->getResourceByID(inputs[(int)DeferredLightingInput::POSITION]->index)->getNativeResource();
		descriptorSet->setTexture(InputName[(int)DeferredLightingInput::POSITION], positon);
		auto normal = m_renderGraph->getResourceByID(inputs[(int)DeferredLightingInput::NORMALS]->index)->getNativeResource();
		descriptorSet->setTexture(InputName[(int)DeferredLightingInput::NORMALS], normal);
		auto pbr = m_renderGraph->getResourceByID(inputs[(int)DeferredLightingInput::PBR]->index)->getNativeResource();
		descriptorSet->setTexture(InputName[(int)DeferredLightingInput::PBR], pbr);
		auto shadowMap = m_renderGraph->getResourceByID(inputs[(int)DeferredLightingInput::ShadowMap]->index)->getNativeResource();
		descriptorSet->setTexture(InputName[(int)DeferredLightingInput::ShadowMap], shadowMap);
		descriptorSet->setTexture("uDepthSampler", std::static_pointer_cast<Texture>(m_renderGraph->depthBuffer));
		descriptorSet->update();
		PipelineInfo pipeInfo;
		pipeInfo.shader = m_RenderDefferredLightingData.deferredLightShader;
		pipeInfo.polygonMode = PolygonMode::Fill;
		pipeInfo.cullMode = CullMode::None;
		pipeInfo.transparencyEnabled = false;
		pipeInfo.depthBiasEnabled = false;
		pipeInfo.clearTargets = false;
		pipeInfo.colorTargets[0] = m_renderGraph->getResourceByID(outputs[0]->index)->getNativeResource();
		pipeInfo.colorTargets[1] = m_renderGraph->getResourceByID(outputs[1]->index)->getNativeResource();
		auto deferredLightPipeline = Pipeline::get(pipeInfo);
		deferredLightPipeline->bind(commandBuffer);
		RenderDevice::bindDescriptorSets(deferredLightPipeline.get(), commandBuffer, 0, m_RenderDefferredLightingData.descriptorLightSet);
		RenderDevice::drawMesh(commandBuffer, deferredLightPipeline.get(), m_RenderDefferredLightingData.screenQuad.get());
		deferredLightPipeline->end(commandBuffer);
	}
	auto RenderDeferredLightingPass::setup() -> void
	{
	}
	auto RenderDeferredLightingPass::onUpdate(entt::registry& registry, std::vector<entt::entity>& culledEnts) -> void
	{
		//PROFILE_FUNCTION();
		//acquire camera data
		auto cameras = registry.group<component::Camera>(entt::get<component::Transform>);
		if (cameras.empty())
		{
			PLOGW("Non Main Camera Detected!");
			return;
		}
		if (cameras.size() > 1)
		{
			PLOGW("More than one Main Camera Detected! Random one will be used !");
		}	
		auto cameraEnt = Entity(cameras.front(), registry);
		auto& camTransform = cameraEnt.getComponent<component::Transform>();
		auto& view = camTransform.getWorldMatrixInverse();
		auto cameraPos = glm::vec4{ camTransform.getWorldPosition(), 1.f };
		//acquire lighting data
		uint32_t  numLights = 0;
		component::Light* directionaLight = nullptr;
		component::LightData lightdatas[32] = {};
		auto lights = registry.group<component::Light>(entt::get< component::Transform>);
		if (!lights.empty())
		{
			for (const auto& data : lights.each())
			{
				auto& [ent, light, transform] = data;
				light.lightData.position = {transform.getWorldPosition(), 1.0f};
				auto worldOrientation = transform.getWorldOrientation();
				light.lightData.direction = { glm::normalize(worldOrientation * pengine::FORWARD) * -1.0f, 1.f };
				if (static_cast<component::LightType>(light.lightData.type) == component::LightType::DirectionalLight)
					directionaLight = &light;
				lightdatas[numLights] = light.lightData;
				numLights++;
			}
		}
		//upload //non common part
		int32_t renderMode = 0;
		m_RenderDefferredLightingData.descriptorLightSet[0]->setUniform("UniformBufferLight", "lights", lightdatas, sizeof(component::LightData) * numLights, false);
		m_RenderDefferredLightingData.descriptorLightSet[0]->setUniform("UniformBufferLight", "cameraPosition", &cameraPos);
		m_RenderDefferredLightingData.descriptorLightSet[0]->setUniform("UniformBufferLight", "viewMatrix", &view);
		m_RenderDefferredLightingData.descriptorLightSet[0]->setUniform("UniformBufferLight", "lightCount", &numLights);
		m_RenderDefferredLightingData.descriptorLightSet[0]->setUniform("UniformBufferLight", "mode", &renderMode);
	}
	auto RenderDeferredLightingPass::onResize(uint32_t width, uint32_t height, uint32_t displayWidth, uint32_t displayHeight) -> void
	{
		//m_RenderDefferredLightingData = RenderDeferredLightingData();
		//temp all the same
		for (int i = 0; i < inputs.size(); i++)
		{
			auto res = inputs[i];
			res->onResize(width, height);
		}
		for (int i = 0; i < outputs.size(); i++)
		{
			auto res = outputs[i];
			res->onResize(width, height);
		}
	}
	auto RenderDeferredLightingPass::createVResource() -> void
	{
		inputs.resize(5);
		outputs.resize(2);
		//temp all the same
		for (int i = 0; i < inputs.size(); i++)
		{
			auto res = new RenderGraphVirtualResource();
			res->type = typeMap[i];;
			res->format = formatMap[i];
			auto renderExtend = m_renderGraph->getRenderExtend();
			res->width = renderExtend.x;
			res->height = renderExtend.y;
			if (i == 4)
			{
				res->width = m_renderGraph->getCommonData().shadowMapSize;
				res->height = m_renderGraph->getCommonData().shadowMapSize;
			}
			inputs[i] = std::shared_ptr<RenderGraphVirtualResource>(res);
		}
		for (int i = 0; i < outputs.size(); i++)
		{
			auto res = new RenderGraphVirtualResource();
			res->type = RenderResouceType::Res_Texture2D;
			res->format = TextureFormat::RGBA16;
			auto renderExtend = m_renderGraph->getRenderExtend();
			res->width = renderExtend.x;
			res->height = renderExtend.y;
			outputs[i] = std::shared_ptr<RenderGraphVirtualResource>(res);
		}
		//temp
		outputs[1]->format = TextureFormat::RGBA32;
	}
	RenderDeferredLightingData::RenderDeferredLightingData()
	{
		//deferredLightShader = Shader::create("F:/workspace/YizhouHu/PEngine/PEngine/assets/shaders/DeferredLight.shader");
		std::string tmpPath = "shaders/DeferredLight.shader";
		deferredLightShader = Shader::create(ASSETS_ABSOLUTE_PATH + tmpPath);
		descriptorLightSet.resize(1);
		DescriptorInfo info{};
		info.shader = deferredLightShader.get();
		info.layoutIndex = 0;
		descriptorLightSet[0] = DescriptorSet::create(info);
		screenQuad = Mesh::createQuad(true);
	}
};