#pragma once
#include <memory>
#include <imgui.h>
namespace pengine
{
	class Texture2D;
	class CommandBuffer;
	class ImGuiRenderer
	{
	public:
		static std::shared_ptr<ImGuiRenderer> create(uint32_t width, uint32_t height, bool clearScreen);
		virtual ~ImGuiRenderer() = default;
		virtual void init() = 0;
		virtual void newFrame(const float& delta) = 0;
		virtual void render(CommandBuffer *commandBuffer) = 0;
		virtual void onResize(uint32_t width, uint32_t height) = 0;
		virtual void rebuildFontTexture() = 0;
		virtual void clear() {};
		virtual ImTextureID addTexture(Texture2D* texture) = 0;
	};
}