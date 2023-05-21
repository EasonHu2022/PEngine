#pragma once
#include "core/core.h"
#include <memory>
#include "glm/glm.hpp"
namespace pengine
{
	class Texture;
	class CommandBuffer;
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
		static auto present() -> void;
		static auto present(CommandBuffer* commandBuffer) -> void;
		virtual auto presentInternal() -> void {};
		virtual auto presentInternal(CommandBuffer* commandBuffer) -> void {};

		static auto create() -> std::shared_ptr<RenderDevice>;
	private:

	};

	
};