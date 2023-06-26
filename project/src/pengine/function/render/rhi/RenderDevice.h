#pragma once
#include "core/core.h"
#include <memory>
#include "glm/glm.hpp"
#include "Defs.h"
namespace pengine
{
	class Texture;
	class CommandBuffer;
	class DescriptorSet;
	class Mesh;
	/// <summary>
	/// on design, this class gives all of native render relative api to engine use;
	/// </summary>
	class PENGINE_API RenderDevice
	{
	public:
		RenderDevice() = default;
		virtual ~RenderDevice() = default;
		virtual void init() = 0;
		virtual void begin() = 0;
		virtual void onResize(uint32_t width, uint32_t height) = 0;
		virtual void drawSplashScreen(const std::shared_ptr<Texture>& texture)  {};
		virtual auto clearRenderTarget(const std::shared_ptr<Texture>& texture, CommandBuffer* commandBuffer, const glm::vec4& clearColor = { 0.3f, 0.3f, 0.3f, 1.0f }) -> void {};
		virtual auto clearRenderTarget(const glm::vec4& clearColor = { 0.3f, 0.3f, 0.3f, 1.0f }) -> void {};
		virtual auto bindDescriptorSetsInternal(Pipeline* pipeline, CommandBuffer* commandBuffer, uint32_t dynamicOffset, const std::vector<std::shared_ptr<DescriptorSet>>& descriptorSets) -> void {};
		virtual auto presentInternal() -> void {};
		virtual auto presentInternal(CommandBuffer* commandBuffer) -> void {};
		virtual auto drawIndexedInternal(CommandBuffer* commandBuffer, DrawType type, uint32_t count, uint32_t start = 0) const -> void {};
		//static interfaces
		static auto present() -> void;
		static auto present(CommandBuffer* commandBuffer) -> void;
		static auto bindDescriptorSets(Pipeline* pipeline, CommandBuffer* commandBuffer, uint32_t dynamicOffset, const std::vector<std::shared_ptr<DescriptorSet>>& descriptorSets) -> void;
		static auto drawIndexed(CommandBuffer* commandBuffer, DrawType type, uint32_t count, uint32_t start = 0) -> void;
		static auto create() -> std::shared_ptr<RenderDevice>;
		static auto drawMesh(CommandBuffer* cmdBuffer, Pipeline* pipeline, Mesh* mesh) -> void;
	private:

	};

	
};