#pragma once
#include "IPass.h"
#include <string>
namespace pengine
{

	enum class DeferredLightingInput
	{
		COLOR = 0,
		POSITION = 1,
		NORMALS = 2,
		PBR = 3,
	};
	static const std::string InputName[] = {"uColorSampler","uPositionSampler","uNormalSampler","uPBRSampler"};
	//for local use
	struct RenderDeferredLightingData
	{
		std::vector<std::shared_ptr<DescriptorSet>> descriptorLightSet;
		std::shared_ptr<Shader> deferredLightShader;
		std::shared_ptr<Mesh>     screenQuad; 
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
		RenderDeferredLightingData m_RenderDefferredLightingData;
	};
};