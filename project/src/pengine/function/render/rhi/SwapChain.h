#pragma once
#include <memory>
namespace pengine
{
	class CommandBuffer;
	class Window;
	class Texture;
	class FrameBuffer;
	class RenderPass;
	class SwapChain
	{
	public:
		static std::shared_ptr<SwapChain> create(uint32_t width, uint32_t height);

		virtual bool init(bool vsync, Window* window) = 0;
		virtual bool init(bool vsync) = 0;
		virtual std::shared_ptr<Texture> getCurrentImage()= 0;
		virtual std::shared_ptr<Texture>  getImage(uint32_t index) = 0;
		virtual uint32_t getCurrentBufferIndex() const = 0;
		virtual uint32_t getCurrentImageIndex() const = 0;
		virtual size_t getSwapChainBufferCount() const = 0;

		virtual auto getCurrentCommandBuffer() -> CommandBuffer*
		{
			return nullptr;
		}

	};
};