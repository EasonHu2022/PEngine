#include "RenderGBufferTask.h"
#include "RenderGraph.h"
#include "function/render/rhi/Pipeline.h"
#include "function/render/rhi/Shader.h"
#include "Application.h"
#include "function/ecs/component/Transform.h"
#include "function/ecs/component/MeshRenderer.h"
#include "function/ecs/component/camera.h"
namespace pengine
{
	RenderGBufferData::RenderGBufferData()
	{
		deferredColorShader = Shader::create("shaders/DeferredColor.shader");
		stencilShader = Shader::create("shaders/Outline.shader");
		MaterialProperties properties;
		properties.albedoColor = glm::vec4(1.f, 1.f, 1.f, 1.f);
		properties.roughnessColor = glm::vec4(0);
		properties.metallicColor = glm::vec4(0);
		properties.usingAlbedoMap = 0.0f;
		properties.usingRoughnessMap = 0.0f;
		properties.usingNormalMap = 0.0f;
		properties.usingMetallicMap = 0.0f;

		defaultMaterial = std::make_shared<Material>(deferredColorShader, properties);
	}

	RenderGBufferTask::RenderGBufferTask(uint32_t _uid, RenderGraph* renderGraph) : ITask(_uid, renderGraph)
	{
		//only for serilization attributions
	}
	RenderGBufferTask::~RenderGBufferTask()
	{
	}
	auto RenderGBufferTask::init(entt::registry& registry) -> void
	{
		//filter the registry entities
		auto meshes = registry.group<component::MeshRenderer, component::Transform>();
		if (!meshes.empty())
		{
			for (const auto& data : meshes.each())
			{
				m_visbileEntity.emplace_back(std::get<0>(data), registry);
			}
		}
		m_LocalData = RenderGBufferData();
	}
	auto RenderGBufferTask::execute(CommandBuffer* commandBuffer) -> void
	{
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
	auto RenderGBufferTask::setup() -> void
	{
	}
	auto RenderGBufferTask::onUpdate(entt::registry& registry) -> void 
	{
		//refill renderqueue
		m_renderQueue.clear();
		PipelineInfo pipelineInfo{};
		pipelineInfo.shader = m_LocalData.deferredColorShader;
		pipelineInfo.polygonMode = PolygonMode::Fill;
		pipelineInfo.blendMode = BlendMode::SrcAlphaOneMinusSrcAlpha;
		pipelineInfo.clearTargets = false;
		pipelineInfo.swapChainTarget = false;
		for (auto ent : m_visbileEntity)
		{
			auto& unit = m_renderQueue.emplace_back();
			unit.mesh = ent.getComponent<component::MeshRenderer>().getMesh().get();
			unit.transform = ent.getComponent<component::Transform>().getWorldMatrix();
			if (unit.mesh->getSubMeshCount() <= 1)
			{
				unit.material = !unit.mesh->getMaterial().empty() ? unit.mesh->getMaterial()[0].get() : m_LocalData.defaultMaterial.get();
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
			unit.pipelineInfo = pipelineInfo;
			
		}
	}

	auto RenderGBufferTask::onResize(uint32_t width, uint32_t height, uint32_t displayWidth, uint32_t displayHeight) -> void
	{
	}
	auto RenderGBufferTask::onSceneElementChange(Entity& ent) -> void
	{
		if (!ent.hasComponent<component::MeshRenderer>() || !ent.hasComponent<component::Transform>())
		{
			//not concern
			return;
		}
		else
		{
			
		}
	}
};