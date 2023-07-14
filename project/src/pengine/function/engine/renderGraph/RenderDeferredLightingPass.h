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
		ShadowMap = 4,
		_count = 5,
	};
	constexpr RenderResouceType typeMap[] = {
		RenderResouceType::Res_Texture2D,//color
		RenderResouceType::Res_Texture2D,//position
		RenderResouceType::Res_Texture2D, //normals
		RenderResouceType::Res_Texture2D, //pbr
		RenderResouceType::Res_DepthArray//shadowMap };
	};

	constexpr TextureFormat formatMap[] = {
		TextureFormat::RGBA16,//color
		TextureFormat::RGBA16,//position
		TextureFormat::RGBA16, //normals
		TextureFormat::RGBA16, //pbr
		TextureFormat::DEPTH //shadowMap };
	};
	constexpr char* InputName[] = {"uColorSampler","uPositionSampler","uNormalSampler","uPBRSampler","uShadowMap"};
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
		auto onUpdate(entt::registry& registry, std::vector<entt::entity>& culledEnts) -> void override;
		auto onResize(uint32_t width, uint32_t height, uint32_t displayWidth, uint32_t displayHeight) -> void;
		auto createVResource() -> void;
	private:
		RenderDeferredLightingData m_RenderDefferredLightingData{};
	};
};