#pragma once	
#include "IPass.h"

#define SHADOW_MAP_CASCADE_COUNT  4
#define SHADOW_MAP_MAX_SIZE  2048
#define SHADOW_MAP_CASCADE_TRANSITION_RANGE 0.5f
namespace pengine
{
	//for local use

	struct RenderShadowMapData
	{
		float cascadeSplitLambda;
		glm::vec4 splitDepth[SHADOW_MAP_CASCADE_COUNT] = {};
		glm::mat4 shadowProjView[SHADOW_MAP_CASCADE_COUNT] = {};
		/*glm::mat4 shadowProj[SHADOW_MAP_CASCADE_COUNT] = {};
		glm::mat4 shadowView[SHADOW_MAP_CASCADE_COUNT] = {};*/

		std::vector<std::shared_ptr<DescriptorSet>> descriptorSet;

		std::shared_ptr<Shader>            shader;
		RenderShadowMapData();
	};
	class Entity;
	struct RenderUnit;
	class RenderShadowMapPass : public IPass
	{
	public:
		RenderShadowMapPass(uint32_t _uid, RenderGraph* renderGraph);
		~RenderShadowMapPass();
		auto init(entt::registry& registry) -> void override;
		//record
		auto execute(CommandBuffer* commandBuffer) -> void override;
		auto setup() -> void override;
		auto onUpdate(entt::registry& registry, std::vector<entt::entity>& culledEnts) -> void override;
		auto onResize(uint32_t width, uint32_t height, uint32_t displayWidth, uint32_t displayHeight) -> void override;
		auto createVResource() -> void;
	private:
		void updateCascades(Entity& camera,Entity& light);
		void updateStaticShadows(Entity& camera, Entity& light);
	private:
		RenderShadowMapData m_localData;
		bool b_hasDirectionalLight;
		std::vector<std::vector<RenderUnit>> m_renderQueues;

	};
#ifdef PENGINE_OPENGL
	constexpr glm::mat4 BIAS_MATRIX = {
		0.5, 0.0, 0.0, 0.0,
		0.0, 0.5, 0.0, 0.0,
		0.0, 0.0, 0.5, 0.0,
		0.5, 0.5, 0.5, 1.0 };
#endif        

#ifdef PENGINE_VULKAN
	constexpr glm::mat4 BIAS_MATRIX = {
		0.5, 0.0, 0.0, 0.0,
		0.0, 0.5, 0.0, 0.0,
		0.0, 0.0, 1.0, 0.0,
		0.5, 0.5, 0.0, 1.0 };
#endif
}