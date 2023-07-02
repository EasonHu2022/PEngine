#pragma once
#include "IPass.h"
namespace pengine
{
	class RenderGraph;
	//for local use
	struct OutputPassData
	{
		std::vector<std::shared_ptr<DescriptorSet>> descriptorOutputSet;
		std::shared_ptr<Shader> outputShader;
		std::shared_ptr<Mesh>     screenQuad;
		OutputPassData();
	};
	class OutputPass : public IPass
	{
	public:
		OutputPass(uint32_t uid, RenderGraph* renderGraph);
		~OutputPass();
		auto init(entt::registry& registry) -> void override;
		auto execute(CommandBuffer* commandBuffer) -> void override;
		auto setup() -> void override;
		auto onUpdate(entt::registry& registry) -> void override;
		auto onResize(uint32_t width, uint32_t height, uint32_t displayWidth, uint32_t displayHeight) -> void;
		auto createVResource() -> void;
	private:
		OutputPassData m_outputPassData{};
	};

};