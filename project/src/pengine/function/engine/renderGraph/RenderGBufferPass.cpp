#include "RenderGBufferPass.h"
#include "RenderGraph.h"
#include "function/render/rhi/Pipeline.h"
#include "function/render/rhi/Shader.h"
#include "Application.h"
#include "function/ecs/component/Transform.h"
#include "function/ecs/component/MeshRenderer.h"
#include "function/ecs/component/camera.h"
#include "function/render/rhi/Texture.h"
namespace pengine
{
	RenderGBufferData::RenderGBufferData(RenderGraph* renderGraph)
	{
		deferredColorShader = Shader::create("F:/workspace/YizhouHu/PEngine/PEngine/assets/shaders/DeferredColor.shader");
		MaterialProperties properties;
		properties.albedoColor = glm::vec4(1.f, 1.f, 1.f, 1.f);
		properties.roughnessColor = glm::vec4(0);
		properties.metallicColor = glm::vec4(0);
		properties.usingAlbedoMap = 0.0f;
		properties.usingRoughnessMap = 0.0f;
		properties.usingNormalMap = 0.0f;
		properties.usingMetallicMap = 0.0f;

		defaultMaterial = std::make_shared<Material>(deferredColorShader, properties);
		defaultMaterial->createDescriptorSet();
		DescriptorInfo info{};
		descriptorColorSet.resize(3);
		info.shader = deferredColorShader.get();
		info.layoutIndex = 0;
		descriptorColorSet[0] = DescriptorSet::create(info);
		depthTest = true;
		if (depthTest)
		{
			auto renderExtend = renderGraph->getRenderExtend();
			depthBuffer = TextureDepth::create(renderExtend.x, renderExtend.y, true);
			depthBuffer->setName("GBuffer-Depth");

		}
	}

	RenderGBufferPass::RenderGBufferPass(uint32_t _uid, RenderGraph* renderGraph) : IPass(_uid, renderGraph)
	{
		prevFrameValid = false;
		name = "GBUFFER PASS";
		//only for serilization attributions
	}
	RenderGBufferPass::~RenderGBufferPass()
	{
	}
	auto RenderGBufferPass::init(entt::registry& registry) -> void
	{
		//create vresources
		createVResource();


		m_LocalData = RenderGBufferData(m_renderGraph);
	}
	auto RenderGBufferPass::execute(CommandBuffer* commandBuffer) -> void
	{
		m_LocalData.descriptorColorSet[0]->update();
		std::shared_ptr<Pipeline> pipeline;
		//traverse queue
		for (auto unit :m_renderQueue)
		{
			pipeline = Pipeline::get(unit.pipelineInfo);
			if (commandBuffer)
				commandBuffer->bindPipeline(pipeline.get());
			else
			{
				PLOGE("Try to use null cmd buffer! Task : {0} execute() ", name);
			}
			//cbuffer
			auto& pushConstants = m_LocalData.deferredColorShader->getPushConstants()[0];
			pushConstants.setValue("transform", &unit.transform);
			m_LocalData.deferredColorShader->bindPushConstants(commandBuffer, pipeline.get());
			//deal with submesh
			if (unit.mesh->getSubMeshCount() > 1)
			{
				auto& materials = unit.mesh->getMaterial();
				auto& indices = unit.mesh->getSubMeshIndex();
				auto start = 0;
				unit.mesh->getVertexBuffer()->bind(commandBuffer, pipeline.get());
				unit.mesh->getIndexBuffer()->bind(commandBuffer);
				for (auto i = 0; i <= indices.size(); i++)
				{
					auto& material = materials[i];
					auto end = i == indices.size() ? unit.mesh->getIndexBuffer()->getCount() : indices[i];
					m_LocalData.descriptorColorSet[1] = material->getDescriptorSet();
					material->bind();
					RenderDevice::bindDescriptorSets(pipeline.get(), commandBuffer, 0, m_LocalData.descriptorColorSet);
					RenderDevice::drawIndexed(commandBuffer, DrawType::Triangle, end - start, start);

					start = end;
				}

				unit.mesh->getVertexBuffer()->unbind();
				unit.mesh->getIndexBuffer()->unbind();
			}
			else
			{
				m_LocalData.descriptorColorSet[1] = unit.material->getDescriptorSet();
				unit.material->bind();
				RenderDevice::bindDescriptorSets(pipeline.get(), commandBuffer, 0, m_LocalData.descriptorColorSet);
				RenderDevice::drawMesh(commandBuffer, pipeline.get(), unit.mesh);
			}
			//if stencil
			if (unit.stencilPipelineInfo.stencilTest)
			{
				auto stencilPipeline = Pipeline::get(unit.stencilPipelineInfo);
				if (commandBuffer)
				{
					commandBuffer->bindPipeline(stencilPipeline.get());
				}				
				else
				{
					PLOGE("Try to use null cmd buffer! in Task : {0},  execute() ", name);
				}
				auto& pushConstants = m_LocalData.stencilShader->getPushConstants()[0];
				pushConstants.setValue("transform", &unit.transform);
				m_LocalData.stencilShader->bindPushConstants(commandBuffer, stencilPipeline.get());
				RenderDevice::bindDescriptorSets(stencilPipeline.get(), commandBuffer, 0, { m_LocalData.stencilDescriptorSet });
				RenderDevice::drawMesh(commandBuffer, pipeline.get(), unit.mesh);
			}
		}
		if (commandBuffer)
		{
			commandBuffer->unbindPipeline();
		}
		else
		{
			PLOGE("Try to use null cmd buffer! in Task : {0},  execute() ", name);
		}
	}
	auto RenderGBufferPass::setup() -> void
	{
	}
	auto RenderGBufferPass::onUpdate(entt::registry& registry) -> void
	{
		//refill renderqueue
		m_renderQueue.clear();
		PipelineInfo pipelineInfo{};
		pipelineInfo.shader = m_LocalData.deferredColorShader;
		pipelineInfo.polygonMode = PolygonMode::Fill;
		pipelineInfo.blendMode = BlendMode::SrcAlphaOneMinusSrcAlpha;
		pipelineInfo.clearTargets = true;
		pipelineInfo.swapChainTarget = false;
		pipelineInfo.depthTest = m_LocalData.depthTest;
		//acquire camera data
		auto cameras = registry.group<component::Camera>(entt::get<component::Transform>);
		if (cameras.empty())
		{
			PLOGW("Non Main Camera Detected!");
		}
		else
		{
			if (cameras.size() > 1)
			{
				PLOGW("More than one Main Camera Detected! Random one will be used !");
			}
			auto cameraEnt = Entity(cameras.front(), registry);
			auto cameraData = cameraEnt.getComponent<component::Camera>();
			auto cameraTransform = cameraEnt.getComponent<component::Transform>();
			auto project = cameraData.getProjection();
			auto view = cameraTransform.getWorldMatrixInverse();
			//process for vulkan
#ifdef PENGINE_VULKAN
			view[1][1] *= -1.0f;
			view[2][2] *= -1.0f;
#endif // 

			auto projView = project * view;
			if (!prevFrameValid)
				prevFrameProjectView = projView;
			m_LocalData.descriptorColorSet[0]->setUniform("UniformBufferObject", "projView", &projView);
			m_LocalData.descriptorColorSet[0]->setUniform("UniformBufferObject", "view", &view);
			m_LocalData.descriptorColorSet[0]->setUniform("UniformBufferObject", "projViewOld", &prevFrameProjectView);
			prevFrameProjectView = projView;
		}
		
		std::vector<Entity> m_visbileEntity;
		//filter the registry entities
		auto meshes = registry.group<component::MeshRenderer>(entt::get< component::Transform>);
		if (!meshes.empty())
		{
			for (const auto& data : meshes.each())
			{
				m_visbileEntity.emplace_back(std::get<0>(data), registry);
			}
		}
		for (auto ent : m_visbileEntity)
		{
			auto& unit = m_renderQueue.emplace_back();
			unit.mesh = ent.getComponent<component::MeshRenderer>().getMesh().get();
			unit.transform = ent.getComponent<component::Transform>().getWorldMatrix();
			if (unit.mesh->getSubMeshCount() <= 1)
			{
				if (!unit.mesh->getMaterial().empty())
				{
					unit.material = unit.mesh->getMaterial()[0].get();
				}
				else
				{
					unit.material =  m_LocalData.defaultMaterial.get();
				}
				unit.material->bind();
			}
			else
			{
				unit.material = nullptr;
			}
			for (int i = 0; i< outputs.size(); i ++)
			{
				if (!outputs[i]->b_initialized)
				{
					PLOGE("Error when setting render targets : task : {0} : outputs : {1} uninitialized !",name, i);
					return;
				}
				pipelineInfo.colorTargets[i] = m_renderGraph->getResourceByID(outputs[i]->index)->getNativeResource();		
				
			}

			if (unit.material != nullptr)
			{
				pipelineInfo.cullMode = unit.material->isFlagOf(Material::RenderFlags::TwoSided) ? CullMode::None : CullMode::Back;
				pipelineInfo.transparencyEnabled = unit.material->isFlagOf(Material::RenderFlags::AlphaBlend);
			}
			else
			{
				pipelineInfo.cullMode = CullMode::Back;
				pipelineInfo.transparencyEnabled = false;
			}
			if (unit.material == nullptr || (m_LocalData.depthTest && unit.material->isFlagOf(Material::RenderFlags::DepthTest)))
			{
				pipelineInfo.depthTarget = std::static_pointer_cast<Texture>(m_LocalData.depthBuffer);
			}

			unit.pipelineInfo = pipelineInfo;
			
		}
		prevFrameValid = true;
	}

	auto RenderGBufferPass::onResize(uint32_t width, uint32_t height, uint32_t displayWidth, uint32_t displayHeight) -> void
	{
	}
	auto RenderGBufferPass::createVResource() -> void
	{
		inputs.resize(4);
		outputs.resize(4);
		//temp all the same
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
	
	}
	RenderGBufferData::RenderGBufferData()
	{
	}
};