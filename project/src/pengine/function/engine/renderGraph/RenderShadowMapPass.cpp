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
		//updateStaticShadows(cameraEnt, lightEnt);
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
			commonData.shadowMapNum = SHADOW_MAP_CASCADE_COUNT;
			commonData.shadowMapSize = SHADOW_MAP_MAX_SIZE;
			commonData.shadowTransforms = m_localData.shadowProjView;
			commonData.splitDepth = m_localData.splitDepth;
			commonData.biasMatrix = BIAS_MATRIX;

			
		}
	}
	auto RenderShadowMapPass::onResize(uint32_t width, uint32_t height, uint32_t displayWidth, uint32_t displayHeight) -> void 
	{
		for (int i = 0; i < outputs.size(); i++)
		{
			auto res = outputs[i];
			res->onResize(SHADOW_MAP_MAX_SIZE, SHADOW_MAP_MAX_SIZE);
			res->type = RenderResouceType::Res_DepthArray;
			res->format = TextureFormat::DEPTH;
			res->layer = SHADOW_MAP_CASCADE_COUNT;
		}
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
	void RenderShadowMapPass::updateStaticShadows(Entity& camera, Entity& light)//https://developer.nvidia.com/gpugems/gpugems3
	{
		auto& lightTransform = light.getComponent<component::Transform>();
		auto& worldOrientation = lightTransform.getWorldOrientation();
		auto& lightDir = glm::normalize(worldOrientation * pengine::FORWARD);
		glm::mat4 lightViewMatrix = glm::lookAt(glm::vec3(0.0f) - lightDir * 5000.0f, glm::vec3(0.0f), pengine::UP);
		glm::mat4 lightOrthoMatrix = glm::ortho(-SHADOW_MAP_MAX_SIZE/2.0f, SHADOW_MAP_MAX_SIZE/2.0f, -SHADOW_MAP_MAX_SIZE/2.0f, SHADOW_MAP_MAX_SIZE/2.0f, 0.1f, (SHADOW_MAP_MAX_SIZE) * 3.0f);
		m_localData.splitDepth[0] = glm::vec4(1.0f) * -1.f;
		m_localData.shadowProjView[0] = lightOrthoMatrix * lightViewMatrix;
	}
	
	void RenderShadowMapPass::updateCascades(Entity& camera, Entity& light)
	{
		auto& cameraC = camera.getComponent<component::Camera>();
		auto& cameraTransform = camera.getComponent<component::Transform>();
		auto& lightC = light.getComponent<component::Light>();
		auto& lightTransform = light.getComponent<component::Transform>();
		float cascadeSplits[SHADOW_MAP_CASCADE_COUNT]{};
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
		auto& iWorldMatrix = cameraTransform.getWorldMatrixInverse();
		auto& identityMatrix = glm::mat4(1.0f);
		for (uint32_t l = 0; l < SHADOW_MAP_CASCADE_COUNT; l++)
		{
			float splitDist = cascadeSplits[l];		
			Frustum camFrustum = cameraC.getFrustum(identityMatrix);//do in camera space
			glm::vec3* frustumCorners = camFrustum.getVertices();
			for (uint32_t i = 0; i < 4; i++)
			{
				glm::vec3 dist = frustumCorners[i + 4] - frustumCorners[i];
				frustumCorners[i + 4] = frustumCorners[i] + (dist * splitDist);
				frustumCorners[i] = frustumCorners[i] + (dist * lastSplitDist);
				//push back near plane a little to do layers transition
				if (i < 3)
				{
					frustumCorners[i + 4] += (frustumCorners[i + 4] - frustumCorners[i]) * SHADOW_MAP_CASCADE_TRANSITION_RANGE;
				}
			}
			//calculate circumcircle center of the subfrustum
			float a = glm::length((frustumCorners[3] - frustumCorners[0]));
			float b = glm::length((frustumCorners[7] - frustumCorners[4]));
			float lengthNF = abs(frustumCorners[4].z - frustumCorners[0].z);
			float center2Near = lengthNF * 0.5f - (a * a - b * b) / (8.0f * lengthNF);
			float length2Camera = abs(frustumCorners[0].z) + center2Near;
			glm::vec3 frustumCenter = glm::vec3(0.0f,0.0f, -length2Camera);
			//fix center
			auto cameraPos = cameraTransform.getWorldPosition();
			glm::vec3 wsFrustumCenter = cameraPos + glm::normalize(cameraTransform.getForwardDirection()) * length2Camera * -1.0f;
			glm::vec4 homoWsFrustumCenter = { wsFrustumCenter.x,wsFrustumCenter.y,wsFrustumCenter.z,1.0f };
			////fix frustum center		
			////https://zhuanlan.zhihu.com/p/515385379
			//keep trans in same space
			lightTransform.setLocalPosition({ 0.0f,2000.0f,0.0f });
			lightTransform.setWorldMatrix(glm::mat4{ 1.f });
			auto& lightViewMat = lightTransform.getWorldMatrixInverse();
			glm::vec4 homoLsFrustumCenter = lightViewMat * homoWsFrustumCenter;
			float radius = 0.0f;
			for (uint32_t i = 0; i < 8; i++)
			{
				float distance = glm::length(frustumCorners[i] - frustumCenter);
				radius = glm::max(radius, distance);
			}
			
			float fCascadeBound = radius * 2.0f;
			float fWorldUnitsPerTexel = fCascadeBound / SHADOW_MAP_MAX_SIZE;
			float fXmod = fmod(homoLsFrustumCenter.x, fWorldUnitsPerTexel);
			float fYmod = fmod(homoLsFrustumCenter.y, fWorldUnitsPerTexel);
			homoLsFrustumCenter = { homoLsFrustumCenter.x - fXmod,homoLsFrustumCenter.y - fYmod,homoLsFrustumCenter.z,homoLsFrustumCenter.w };
			//get frustumcenter back to world space
			auto& lightWorldMat = lightTransform.getWorldMatrix();
			homoWsFrustumCenter = lightWorldMat * homoLsFrustumCenter;
			frustumCenter = { homoWsFrustumCenter.x,homoWsFrustumCenter.y,homoWsFrustumCenter.z };	
			//calculate light vp
			auto& worldOrientation = lightTransform.getWorldOrientation();			
			auto& lightDir = glm::normalize(worldOrientation * pengine::FORWARD) * 1.0f;
			lightTransform.setLocalPosition(frustumCenter - lightDir * radius * 5.0f);
			lightTransform.setWorldMatrix(glm::mat4{ 1.f });
			glm::mat4 lightViewMatrix = lightTransform.getWorldMatrixInverse();
			glm::mat4 lightOrthoMatrix = glm::ortho(-radius, radius, -radius, radius, 0.f, 2.0f* radius *10 );
			m_localData.splitDepth[l] = glm::vec4(nearClip + splitDist * clipRange) * -1.f;
			m_localData.shadowProjView[l] = lightOrthoMatrix * lightViewMatrix;
			lastSplitDist = splitDist;
			m_localData.splitDepth[l].y += l == 0 ? (m_localData.splitDepth[l].x + nearClip) * SHADOW_MAP_CASCADE_TRANSITION_RANGE : (m_localData.splitDepth[l].x - m_localData.splitDepth[l - 1].x) * SHADOW_MAP_CASCADE_TRANSITION_RANGE;
		}
	}
	RenderShadowMapData::RenderShadowMapData()
	{	
		cascadeSplitLambda = 0.995f;
		std::string tempPath = "shaders/CascadeShadow.shader";
		shader = Shader::create(Application::getAssetsManager()->GetInternalAssetsPath() + tempPath);
		DescriptorInfo createInfo{};
		createInfo.layoutIndex = 0;
		createInfo.shader = shader.get();
		descriptorSet.resize(1);
		descriptorSet[0] = DescriptorSet::create(createInfo);
	}
};