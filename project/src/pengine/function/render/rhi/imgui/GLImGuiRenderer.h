#pragma once
#include "ImGuiRenderer.h"
namespace pengine
{
	class GLImGuiRenderer : public ImGuiRenderer
	{
	public:
		GLImGuiRenderer();
		~GLImGuiRenderer();

	private:

		virtual void init() override;

		virtual void newFrame(const float& delta) override;

		virtual void render(CommandBuffer* commandBuffer) override;

		virtual void onResize(uint32_t width, uint32_t height) override;

		virtual void rebuildFontTexture() override;

		virtual ImTextureID addTexture(Texture2D* texture) override;
	};
}