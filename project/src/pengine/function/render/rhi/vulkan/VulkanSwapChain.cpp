#include "VulkanSwapChain.h"
#include "VulkanContext.h"
#include "function/window/Window.h"
#include "VulkanDevice.h"
#include "VulkanCommandBuffer.h"
#include "VulkanCommmandPool.h"
#include "VulkanTexture.h"

#include <GLFW/glfw3.h> // must include after include vulkan because some key macro should be pre defined
#include <GLFW/glfw3native.h>
namespace pengine
{
	namespace
	{
		inline auto createPlatformSurface(Window* window)
		{
			VkSurfaceKHR surface;
			glfwCreateWindowSurface(VulkanContext::get()->getVkInstance(), static_cast<GLFWwindow*>(window->getNativeInterface()), nullptr, (VkSurfaceKHR*)&surface);
			return surface;
		}
	};

	VulkanSwapChain::VulkanSwapChain(uint32_t width, uint32_t height) :
		width(width), height(height)
	{
	}

	VulkanSwapChain::~VulkanSwapChain()
	{
		
	}
	auto VulkanSwapChain::release() -> void
	{
		releaseResources();
		vkDestroySwapchainKHR(*VulkanDevice::get(), swapChain, VK_NULL_HANDLE);
		if (surface != VK_NULL_HANDLE)
		{
			vkDestroySurfaceKHR(VulkanContext::get()->getVkInstance(), surface, nullptr);
		}
	}
	auto VulkanSwapChain::releaseResources() -> void 
	{
		swapChainBuffers.clear();
		for (uint32_t i = 0; i < swapChainBufferCount; i++)
		{
			vkDestroySemaphore(*VulkanDevice::get(), frames[i].presentSemaphore, nullptr);
			frames[i].commandBuffer->flush();
			frames[i].commandBuffer = nullptr;
		}	
	}

	bool VulkanSwapChain::init(bool vsync, Window* window)
	{
		this->vsync = vsync;

		if (surface == VK_NULL_HANDLE)
			surface = createPlatformSurface(window);

		bool success = init(vsync);
		return success;
	}
	bool VulkanSwapChain::init(bool vsync)
	{
		
		findImageFormatAndColorSpace();
		if (!surface)
		{
			PLOGE("[VULKAN] Failed to create window surface!");
		}
		VkBool32 queueIndexSupported;
		vkGetPhysicalDeviceSurfaceSupportKHR(*VulkanDevice::get()->getPhysicalDevice(),
			VulkanDevice::get()->getPhysicalDevice()->getQueueFamilyIndices().graphicsFamily.value(),
			surface, &queueIndexSupported);

		if (queueIndexSupported == VK_FALSE)
			PLOGE("Graphics Queue not supported");

			// Swap chain
		VkSurfaceCapabilitiesKHR surfaceCapabilities;
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(*VulkanDevice::get()->getPhysicalDevice(), surface, &surfaceCapabilities);

		uint32_t numPresentModes;
		vkGetPhysicalDeviceSurfacePresentModesKHR(*VulkanDevice::get()->getPhysicalDevice(), surface, &numPresentModes, VK_NULL_HANDLE);

		std::vector<VkPresentModeKHR> pPresentModes(numPresentModes);
		vkGetPhysicalDeviceSurfacePresentModesKHR(*VulkanDevice::get()->getPhysicalDevice(), surface, &numPresentModes, pPresentModes.data());

		VkExtent2D swapChainExtent;

		swapChainExtent.width = static_cast<uint32_t>(width);
		swapChainExtent.height = static_cast<uint32_t>(height);

		auto swapChainPresentMode = VKHelper::chooseSwapPresentMode(pPresentModes, vsync);

		//triple-buffering
		swapChainBufferCount = surfaceCapabilities.maxImageCount;

		if (swapChainBufferCount > 3)
			swapChainBufferCount = 3;

		VkSurfaceTransformFlagBitsKHR preTransform;
		if (surfaceCapabilities.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR)
		{
			preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
		}
		else
		{
			preTransform = surfaceCapabilities.currentTransform;
		}

		VkCompositeAlphaFlagBitsKHR              compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		std::vector<VkCompositeAlphaFlagBitsKHR> compositeAlphaFlags = {
			VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
			VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR,
			VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR,
			VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR,
		};

		for (auto& compositeAlphaFlag : compositeAlphaFlags)
		{
			if (surfaceCapabilities.supportedCompositeAlpha & compositeAlphaFlag)
			{
				compositeAlpha = compositeAlphaFlag;
				break;
			};
		}

		VkSwapchainCreateInfoKHR swapChainCI{};
		swapChainCI.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		swapChainCI.surface = surface;
		swapChainCI.minImageCount = swapChainBufferCount;
		swapChainCI.imageFormat = colorFormat;
		swapChainCI.imageExtent.width = swapChainExtent.width;
		swapChainCI.imageExtent.height = swapChainExtent.height;
		swapChainCI.preTransform = preTransform;
		swapChainCI.compositeAlpha = compositeAlpha;
		swapChainCI.imageArrayLayers = 1;
		swapChainCI.presentMode = swapChainPresentMode;
		swapChainCI.oldSwapchain = oldSwapChain;
		swapChainCI.imageColorSpace = colorSpace;
		swapChainCI.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		swapChainCI.queueFamilyIndexCount = 0;
		swapChainCI.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		swapChainCI.pQueueFamilyIndices = VK_NULL_HANDLE;
		swapChainCI.clipped = VK_TRUE;


		if (surfaceCapabilities.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_SRC_BIT)
		{
			swapChainCI.imageUsage |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
		}

		if (surfaceCapabilities.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_DST_BIT)
		{
			swapChainCI.imageUsage |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
		}

		VK_CHECK_RESULT(vkCreateSwapchainKHR(*VulkanDevice::get(), &swapChainCI, VK_NULL_HANDLE, &swapChain));

		if (oldSwapChain != VK_NULL_HANDLE)
		{
			vkDestroySwapchainKHR(*VulkanDevice::get(), oldSwapChain, VK_NULL_HANDLE);
			oldSwapChain = VK_NULL_HANDLE;
		}

		uint32_t swapChainImageCount;
		VK_CHECK_RESULT(vkGetSwapchainImagesKHR(*VulkanDevice::get(), swapChain, &swapChainImageCount, VK_NULL_HANDLE));

		VkImage* pSwapChainImages = new VkImage[swapChainImageCount];
		VK_CHECK_RESULT(vkGetSwapchainImagesKHR(*VulkanDevice::get(), swapChain, &swapChainImageCount, pSwapChainImages));

		for (uint32_t i = 0; i < swapChainBufferCount; i++)
		{
			VkImageViewCreateInfo viewCI{};
			viewCI.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			viewCI.format = colorFormat;
#ifdef PLATFORM_MACOS
			viewCI.components = { VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY };
#else
			viewCI.components = { VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A };
#endif
			viewCI.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			viewCI.subresourceRange.baseMipLevel = 0;
			viewCI.subresourceRange.levelCount = 1;
			viewCI.subresourceRange.baseArrayLayer = 0;
			viewCI.subresourceRange.layerCount = 1;
			viewCI.viewType = VK_IMAGE_VIEW_TYPE_2D;
			viewCI.flags = 0;
			viewCI.image = pSwapChainImages[i];

			VkImageView imageView;
			VK_CHECK_RESULT(vkCreateImageView(*VulkanDevice::get(), &viewCI, VK_NULL_HANDLE, &imageView));
			auto swapChainBuffer = std::make_shared < VulkanTexture2D > (pSwapChainImages[i], imageView, colorFormat, width, height);
			swapChainBuffer->transitionImage(VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
			swapChainBuffers.emplace_back(swapChainBuffer);
		}

		delete[] pSwapChainImages;
		createFrameData();
		return true;
		
	}
	void VulkanSwapChain::createFrameData()
	{
		for (uint32_t i = 0; i < swapChainBufferCount; i++)
		{
			if (!frames[i].commandBuffer)
			{
				VkSemaphoreCreateInfo semaphoreInfo = {};
				semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
				semaphoreInfo.pNext = nullptr;
				semaphoreInfo.flags = 0;

				if (frames[i].presentSemaphore == VK_NULL_HANDLE)
					VK_CHECK_RESULT(vkCreateSemaphore(*VulkanDevice::get(), &semaphoreInfo, nullptr, &frames[i].presentSemaphore));

				frames[i].commandPool = std::make_shared<VulkanCommandPool>(VulkanDevice::get()->getPhysicalDevice()->getQueueFamilyIndices().graphicsFamily.value(),
					VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);

				frames[i].commandBuffer = std::make_shared<VulkanCommandBuffer>();
				frames[i].commandBuffer->init(true, *frames[i].commandPool);
				PLOGI("Create the VulkanCommandBuffer");
			}
		}
	}
	void VulkanSwapChain::findImageFormatAndColorSpace()
	{
		VkPhysicalDevice physicalDevice = *VulkanDevice::get()->getPhysicalDevice();
		uint32_t formatCount;
		VK_CHECK_RESULT(vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, NULL));
		PENGINE_ASSERT(formatCount > 0, "");
		std::vector<VkSurfaceFormatKHR> surfaceFormats(formatCount);
		VK_CHECK_RESULT(vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, surfaceFormats.data()));
		if ((formatCount == 1) && (surfaceFormats[0].format == VK_FORMAT_UNDEFINED))
		{
			colorFormat = VK_FORMAT_B8G8R8A8_UNORM;        //use this one as default when there is no surface format
			colorSpace = surfaceFormats[0].colorSpace;
		}
		else
		{
			bool flag = false;
			for (auto&& surfaceFormat : surfaceFormats)        //VK_FORMAT_B8G8R8A8_UNORM
			{
				if (surfaceFormat.format == VK_FORMAT_B8G8R8A8_UNORM)
				{
					colorFormat = surfaceFormat.format;
					colorSpace = surfaceFormat.colorSpace;
					flag = true;
					break;
				}
			}

			// if VK_FORMAT_B8G8R8A8_UNORM is not available, select the first available as default
			if (!flag)
			{
				colorFormat = surfaceFormats[0].format;
				colorSpace = surfaceFormats[0].colorSpace;
			}
		}
	}
	void VulkanSwapChain::queueSubmit()
	{
		auto& frameData = getFrameData();
		frameData.commandBuffer->executeInternal(VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,frameData.presentSemaphore,true);
	}
	void VulkanSwapChain::begin()
	{
		acquireNextImage();
		auto commandBuffer = getFrameData().commandBuffer;
		if (commandBuffer->getState() == CommandBufferState::Submitted)
		{
			commandBuffer->wait();
		}
		commandBuffer->reset();
		VulkanContext::getDeletionQueue(currentBuffer).flush();
		commandBuffer->beginRecording();
		
	}
	void VulkanSwapChain::end()
	{
		getCurrentCommandBuffer()->endRecording();
	}
	CommandBuffer* VulkanSwapChain::getCurrentCommandBuffer()
	{
		return getFrameData().commandBuffer.get();
	}
	void VulkanSwapChain::onResize(uint32_t width, uint32_t height, bool forceResize, Window* windowHandle)
	{
		if (!forceResize && this->width == width && this->height == height)
			return;

		VulkanContext::get()->waitIdle();

		this->width = width;
		this->height = height;

		for (uint32_t i = 0; i < swapChainBufferCount; i++)
		{
			if (frames[i].commandBuffer->getState() == CommandBufferState::Submitted)
				frames[i].commandBuffer->wait();

			swapChainBuffers[i].reset();
		}
		swapChainBuffers.clear();
		oldSwapChain = swapChain;
		swapChain = VK_NULL_HANDLE;

		if (windowHandle)
		{
			init(vsync, windowHandle);
		}
		else
		{
			init(vsync);	
			
		}
	}
	void VulkanSwapChain::acquireNextImage()
	{	
		if (swapChainBufferCount == 1 && acquireImageIndex != std::numeric_limits<uint32_t>::max())
			return;
		{
			auto result = vkAcquireNextImageKHR(*VulkanDevice::get(), swapChain, UINT64_MAX, frames[currentBuffer].presentSemaphore, VK_NULL_HANDLE, &acquireImageIndex);

			if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
			{
				PLOGE("Acquire Image result : {0}", result == VK_ERROR_OUT_OF_DATE_KHR ? "Out of Date" : "SubOptimal");

				if (result == VK_ERROR_OUT_OF_DATE_KHR)
				{
					onResize(width, height, true);
					
				}
				return;
			}
			else if (result != VK_SUCCESS)
			{
				PLOGE("[VULKAN] Failed to acquire swap chain image!");
			}
		}
	}
	void VulkanSwapChain::present(VkSemaphore waitSemaphore)
	{
		VkPresentInfoKHR present;
		present.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		present.pNext = VK_NULL_HANDLE;
		present.swapchainCount = 1;
		present.pSwapchains = &swapChain;
		present.pImageIndices = &acquireImageIndex;
		present.waitSemaphoreCount = 1;
		present.pWaitSemaphores = &waitSemaphore;
		present.pResults = VK_NULL_HANDLE;

		auto error = vkQueuePresentKHR(VulkanDevice::get()->getPresentQueue(), &present);

		if (error == VK_ERROR_OUT_OF_DATE_KHR)
		{
			PLOGE("[Vulkan] SwapChain is out of date, plz handle window iconify event ASAP!");
		}
		else if (error == VK_SUBOPTIMAL_KHR)
		{
			PLOGE("[Vulkan] SwapChain suboptimal....");
		}
		else
		{
			VK_CHECK_RESULT(error);
		}
		//wait
		//VulkanContext::getDeletionQueue(currentBuffer).flush();
		//update current buffer
		currentBuffer = (currentBuffer + 1) % swapChainBufferCount;
	}
	FrameData& VulkanSwapChain::getFrameData()
	{
		PENGINE_ASSERT(currentBuffer < swapChainBufferCount, "buffer index is out of bounds");
		return frames[currentBuffer];
	}
};