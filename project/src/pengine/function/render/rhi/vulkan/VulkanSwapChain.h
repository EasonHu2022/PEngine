#pragma once
#include "VKHelper.h"
#include "core/core.h"
#include "function/render/rhi/SwapChain.h"
#include "function/render/rhi/Texture.h"
#include "VulkanCommandBuffer.h"
#include "VulkanCommmandPool.h"
namespace pengine
{	
	constexpr uint32_t MAX_SWAPCHAIN_BUFFERS = 3;
	struct FrameData
	{
		std::shared_ptr<VulkanCommandPool>   commandPool;
		VkSemaphore                          presentSemaphore = VK_NULL_HANDLE;
		std::shared_ptr<VulkanCommandBuffer> commandBuffer;
	};
	class VulkanSwapChain : public SwapChain
	{
	public:
		VulkanSwapChain(uint32_t width, uint32_t height);
		virtual ~VulkanSwapChain();
		NO_COPYABLE(VulkanSwapChain);
		bool init(bool vsync, Window* window);
		bool init(bool vsync);
		std::shared_ptr<Texture>  getCurrentImage() override
		{
			return swapChainBuffers[acquireImageIndex];
		}
		std::shared_ptr<Texture> getImage(uint32_t index)  override
		{
			return swapChainBuffers[index];
		}
		uint32_t getCurrentBufferIndex() const  override
		{
			return currentBuffer;
		}
		uint32_t getCurrentImageIndex() const override
		{
			return acquireImageIndex;
		}
		size_t getSwapChainBufferCount() const  override
		{
			return swapChainBufferCount;
		}

		inline auto getScreenFormat() const
		{
			return colorFormat;
		}
		inline auto getFormat()
		{
			return colorFormat;
		}

		inline auto getSurface() const
		{
			return surface;
		}

		void queueSubmit() ;
		void begin() ;
		void end() ;

		CommandBuffer* getCurrentCommandBuffer() override;

		void onResize(uint32_t width, uint32_t height, bool forceResize = false, Window* windowHandle = nullptr) ;

		void acquireNextImage() ;

		void present(VkSemaphore waitSemaphore) ;

		FrameData& getFrameData();
		inline operator auto() const 
		{                            
			return swapChain;                
		}


	private:
		void createFrameData();
		FrameData frames[MAX_SWAPCHAIN_BUFFERS];
		void findImageFormatAndColorSpace();
		std::vector<std::shared_ptr<Texture2D>> swapChainBuffers;
		uint32_t currentBuffer = 0;
		uint32_t acquireImageIndex = -1;
		uint32_t queueNodeIndex = -1;
		uint32_t width = 0;
		uint32_t height = 0;
		uint32_t swapChainBufferCount = 0;
		bool     vsync = false;

		VkSwapchainKHR  swapChain = VK_NULL_HANDLE;
		VkSwapchainKHR  oldSwapChain = VK_NULL_HANDLE;
		VkSurfaceKHR    surface = VK_NULL_HANDLE;
		VkFormat        colorFormat;
		VkColorSpaceKHR colorSpace;

	};

};