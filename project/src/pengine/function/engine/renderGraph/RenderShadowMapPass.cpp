#include "RenderShadowMapPass.h"
#include "RenderGraph.h"
#include "function/ecs/component/Transform.h"
#include "function/ecs/component/Light.h"
#include "function/ecs/component/Camera.h"
#include "function/ecs/component/MeshRenderer.h"
#include "function/render/rhi/Shader.h"
#include "function/render/rhi/Texture.h"
#include "function/render/rhi/Pipeline.h"
#include"function/render/rhi/RenderDevice.h"
#include "Application.h"
namespace pengine
{
	RenderShadowMapPass::RenderShadowMapPass(uint32_t _uid, RenderGraph* renderGraph) : IPass(_uid, renderGraph)
	{
		name = "Render Shadowmap Pass";
	}
	RenderShadowMapPass::~RenderShadowMapPass()
	{
	}
	auto RenderShadowMapPass::init(entt::registry& registry) -> void 
	{
		//create vresources
		createVResource();
		b_hasDirectionalLight = false;
		m_renderQueues.resize(SHADOW_MAP_CASCADE_COUNT);
		m_localData = RenderShadowMapData();
	}
	auto RenderShadowMapPass::execute(CommandBuffer* commandBuffer) -> void 
	{
		if (!b_hasDirectionalLight) return;//no directional light
		m_localData.descriptorSet[0]->update();
		PipelineInfo pipelineInfo;
		pipelineInfo.shader = m_localData.shader;

		pipelineInfo.cullMode = CullMode::Back;
		pipelineInfo.transparencyEnabled = false;
		pipelineInfo.depthBiasEnabled = false;
		pipelineInfo.depthArrayTarget = m_renderGraph->getResourceByID(outputs[0]->index)->getNativeResource();
		pipelineInfo.clearTargets = true;
		pipelineInfo.depthTest = true;
		auto pipeline = Pipeline::get(pipelineInfo);
		for (int i = 0; i < SHADOW_MAP_CASCADE_COUNT; i++)
		{
			pipeline->bind(commandBuffer,i);
			auto& cQueue = m_renderQueues[i];
			for (auto& unit : cQueue)
			{
				Mesh* mesh = unit.mesh;
				auto transMat = unit.transform;
				auto& pushConstants = m_localData.shader->getPushConstants()[0];
				pushConstants.setValue("transform", (void*)&transMat);
				pushConstants.setValue("cascadeIndex", (void*)&i);
				m_localData.shader->bindPushConstants(commandBuffer,pipeline.get());
				RenderDevice::bindDescriptorSets(pipeline.get(), commandBuffer, 0, m_localData.descriptorSet);
				RenderDevice::drawMesh(commandBuffer, pipeline.get(), mesh);
			}
			pipeline->end(commandBuffer);
		}
	}
	auto RenderShadowMapPass::setup() -> void 
	{
		
	}
	auto RenderShadowMapPass::onUpdate(entt::registry& registry, std::vector<entt::entity>& culledEnts) -> void 
	{
		//clear renderQueues
		for (int i = 0; i < SHADOW_MAP_CASCADE_COUNT; i++)
		{
			m_renderQueues[i].clear();
		}
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
		//query directional light
		Entity lightEnt{};
		auto lights = registry.group<component::Light>(entt::get< component::Transform>);
		if (!lights.empty())
		{
			for (const auto& data : lights.each())
			{
				auto& [ent, light, transform] = data;
				if (light.lightData.type == (int)component::LightType::DirectionalLight)
				{
					lightEnt = Entity(ent, registry);
					b_hasDirectionalLight = true;
					break;
				}
			}
		}
		if (!b_hasDirectionalLight) return;//no directional light
		updateCascades(cameraEnt, lightEnt);
		//query meshes
		//cull for each light frustum for each level and fill render queue
		//so we need SHADOW_MAP_CASCADE_COUNT render queues for each level
		auto meshes = registry.group<component::MeshRenderer>(entt::get< component::Transform>);
		if (!meshes.empty())
		{
			for (int i = 0; i < SHADOW_MAP_CASCADE_COUNT; i++)
			{
				
				//build light frustum
				Frustum lightFrusum{};
				lightFrusum.from(m_localData.shadowProjView[i]);
				for (const auto& data : meshes.each())
				{
					auto& [ent, meshRenderer, transform] = data;
					auto mesh = meshRenderer.getMesh().get();
					auto inside = lightFrusum.isInside(mesh->getBoundingBox());
					if (inside)
					{
						auto& unit = m_renderQueues[i].emplace_back();
						unit.mesh = mesh;
						unit.transform = transform.getWorldMatrix();
						if (unit.mesh->getSubMeshCount() <= 1)
						{
							unit.material = !mesh->getMaterial().empty() ? mesh->getMaterial()[0].get() : nullptr;
						}
						else
						{
							unit.material = nullptr;
						}
					}
				}

			}
			m_localData.descriptorSet[0]->setUniform("UniformBufferObject","projView", m_localData.shadowProjView);
			//fill the shared datas
			auto& commonData = m_renderGraph->getCommonData();
			commonData.lightView = m_localData.lightMatrix;
			commonData.shadowMapNum = SHADOW_MAP_CASCADE_COUNT;
			commonData.shadowMapSize = SHADOW_MAP_MAX_SIZE;
			commonData.shadowTransforms = m_localData.shadowProjView;
			commonData.splitDepth = m_localData.splitDepth;
			commonData.biasMatrix = BIAS_MATRIX;

			
		}
	}
	auto RenderShadowMapPass::onResize(uint32_t width, uint32_t height, uint32_t displayWidth, uint32_t displayHeight) -> void 
	{
		
	}
	auto RenderShadowMapPass::createVResource() -> void
	{
		outputs.resize(1);
		for (int i = 0; i < outputs.size(); i++)
		{
			auto res = new RenderGraphVirtualResource();
			res->type = RenderResouceType::Res_DepthArray;
			res->format = TextureFormat::DEPTH;
			res->width = SHADOW_MAP_MAX_SIZE;
			res->height = SHADOW_MAP_MAX_SIZE;
			res->layer = SHADOW_MAP_CASCADE_COUNT;
			outputs[i] = std::shared_ptr<RenderGraphVirtualResource>(res);
		}
	}
	void RenderShadowMapPass::updateCascades(Entity& camera, Entity& light)//https://developer.nvidia.com/gpugems/gpugems3
	{
		auto& cameraC = camera.getComponent<component::Camera>();
		auto& cameraTransform = camera.getComponent<component::Transform>();
		auto& lightC = light.getComponent<component::Light>();
		auto& lightTransform = light.getComponent<component::Transform>();

		float cascadeSplits[SHADOW_MAP_CASCADE_COUNT];
		float nearClip = cameraC.getNear();
		float farClip = cameraC.getFar();
		float clipRange = farClip - nearClip;
		float minZ = nearClip;
		float maxZ = nearClip + farClip;
		float range = maxZ - minZ;
		float ratio = maxZ / minZ;
		//caculate cascade
		//https://developer.nvidia.com/gpugems/GPUGems3/gpugems3_ch10.html
		for (uint32_t i = 0; i < SHADOW_MAP_CASCADE_COUNT; i++) {
			float p = (i + 1) / static_cast<float>(SHADOW_MAP_CASCADE_COUNT);
			float log = minZ * std::pow(ratio, p);
			float uniform = minZ + range * p;
			float d = m_localData.cascadeSplitLambda * (log - uniform) + uniform;
			cascadeSplits[i] = (d - nearClip) / clipRange;
		}
		float lastSplitDist = 0.0f;
		for (uint32_t i = 0; i < SHADOW_MAP_CASCADE_COUNT; i++)
		{
			float splitDist = cascadeSplits[i];
			Frustum camFrustum = cameraC.getFrustum(cameraTransform.getWorldMatrixInverse());
			glm::vec3* frustumCorners = camFrustum.getVertices();
			for (uint32_t i = 0; i < 4; i++)
			{
				glm::vec3 dist = frustumCorners[i + 4] - frustumCorners[i];
				frustumCorners[i + 4] = frustumCorners[i] + (dist * splitDist);
				frustumCorners[i] = frustumCorners[i] + (dist * lastSplitDist);
			}
			glm::vec3 frustumCenter = glm::vec3(0.0f);
			for (uint32_t i = 0; i < 8; i++)
			{
				frustumCenter += frustumCorners[i];
			}
			frustumCenter /= 8.0f;
			float radius = 0.0f;
			for (uint32_t i = 0; i < 8; i++)
			{
				float distance = glm::length(frustumCorners[i] - frustumCenter);
				radius = glm::max(radius, distance);
			}
			radius = std::ceil(radius * 16.0f) / 16.0f;

			glm::vec3 maxExtents = glm::vec3(radius);
			glm::vec3 minExtents = -maxExtents;

			glm::vec3 lightDir = glm::normalize(glm::vec3(lightC.lightData.direction));
			glm::mat4 lightViewMatrix = glm::lookAt(frustumCenter - lightDir * -minExtents.z, frustumCenter, UP);
			glm::mat4 lightOrthoMatrix = glm::ortho(minExtents.x, maxExtents.x, minExtents.y, maxExtents.y, 0.0f, maxExtents.z - minExtents.z);
			m_localData.splitDepth[i] = glm::vec4(nearClip + splitDist * clipRange) * -1.f;
			m_localData.shadowProjView[i] = lightOrthoMatrix * lightViewMatrix;
			lastSplitDist = splitDist;
			if (i == 0)
			{
				m_localData.lightMatrix = lightViewMatrix;
				m_localData.lightDir = lightDir;
			}
		}
	}
	RenderShadowMapData::RenderShadowMapData()
	{	
		cascadeSplitLambda = 0.995f;
		lightMatrix = {};
		shadowProj = {};
		lightDir = {};
		std::string tempPath = "shaders/CascadeShadow.shader";
		shader = Shader::create(ASSETS_ABSOLUTE_PATH + tempPath);
		DescriptorInfo createInfo{};
		createInfo.layoutIndex = 0;
		createInfo.shader = shader.get();
		descriptorSet.resize(1);
		descriptorSet[0] = DescriptorSet::create(createInfo);
	}
};