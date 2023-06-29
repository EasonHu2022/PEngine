#pragma once
#include "IPass.h"
namespace pengine
{
	//for local use
	struct RenderDeferredLightingData
	{
		std::vector<std::shared_ptr<DescriptorSet>> descriptorLightSet;
		std::shared_ptr<DescriptorSet> stencilDescriptorSet;
		std::shared_ptr<Shader> deferredColorShader;        //stage 0 get all color information     //stage 1 process lighting
		std::shared_ptr<Shader> stencilShader;
		RenderDeferredLightingData();
	};
	class RenderDeferredLightingPass : public IPass
	{
	public:
		RenderDeferredLightingPass(uint32_t uid, RenderGraph* renderGraph);
		~RenderDeferredLightingPass();
		auto init(entt::registry& registry) -> void override;
		auto execute(CommandBuffer* commandBuffer) -> void override;
		auto setup() -> void override;
		auto onUpdate(entt::registry& registry) -> void override;
		auto onResize(uint32_t width, uint32_t height, uint32_t displayWidth, uint32_t displayHeight) -> void;
		auto createVResource() -> void;
	private:
	};
};