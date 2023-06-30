#pragma once
#include "IPass.h"
namespace pengine
{

	//for local use
	struct RenderUnit
	{
		Mesh* mesh = nullptr;
		Material* material = nullptr;
		PipelineInfo pipelineInfo;
		PipelineInfo stencilPipelineInfo;
		glm::mat4 transform;
	};
	//for local use
	struct RenderGBufferData
	{
		std::shared_ptr<Material>                   defaultMaterial;
		std::vector<std::shared_ptr<DescriptorSet>> descriptorColorSet;
		std::shared_ptr<DescriptorSet> stencilDescriptorSet;
		std::shared_ptr<Shader> deferredColorShader;        //stage 0 get all color information     //stage 1 process lighting
		std::shared_ptr<Shader> stencilShader;
		bool depthTest;
		std::shared_ptr<TextureDepth>                                   depthBuffer;
		RenderGBufferData();
		RenderGBufferData(RenderGraph* renderGraph);
	};

	struct SharedRenderData;
	class RenderGBufferPass : public IPass
	{
	public:
		RenderGBufferPass(uint32_t	_uid, RenderGraph* renderGraph);
		~RenderGBufferPass();
		
		auto init(entt::registry& registry) -> void override;
		//record
		auto execute(CommandBuffer* commandBuffer) -> void override;
		auto setup() -> void override;
		auto onUpdate(entt::registry& registry) -> void override;
		auto onResize(uint32_t width, uint32_t height, uint32_t displayWidth, uint32_t displayHeight) -> void override;
		auto createVResource() -> void;
	private:
		
		RenderGBufferData m_LocalData;
		std::vector<RenderUnit> m_renderQueue;
		glm::mat4 prevFrameProjectView;
		bool prevFrameValid;;
	};

};