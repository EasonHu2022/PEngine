#include "VulkanRenderDevice.h"
#include "Application.h"
#include "VulkanSwapChain.h"
#include "VulkanDevice.h"
#include "VulkanTexture.h"
namespace pengine
{
	static constexpr uint32_t MAX_DESCRIPTOR_SET_COUNT = 1500;
	VulkanRenderDevice::VulkanRenderDevice()
	{
	}
	VulkanRenderDevice::~VulkanRenderDevice()
	{
		vkDestroyDescriptorPool(*VulkanDevice::get(), descriptorPool, VK_NULL_HANDLE);
	}
	void VulkanRenderDevice::begin()
	{
		//begin record;
		auto swapChain = Application::getGraphicsContext()->getSwapChain();
		std::static_pointer_cast<VulkanSwapChain>(swapChain)->begin();
		clearRenderTarget();
	}
	void VulkanRenderDevice::init()
	{
		std::array<VkDescriptorPoolSize, 5> poolSizes = {
			VkDescriptorPoolSize{VK_DESCRIPTOR_TYPE_SAMPLER, 100},
			VkDescriptorPoolSize{VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 100},
			VkDescriptorPoolSize{VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 100},
			VkDescriptorPoolSize{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 100},
			VkDescriptorPoolSize{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 100} };

		// Create info
		VkDescriptorPoolCreateInfo poolCreateInfo = {};
		poolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		poolCreateInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
		poolCreateInfo.poolSizeCount = poolSizes.size();
		poolCreateInfo.pPoolSizes = poolSizes.data();
		poolCreateInfo.maxSets = MAX_DESCRIPTOR_SET_COUNT;

		// Pool
		VK_CHECK_RESULT(vkCreateDescriptorPool(*VulkanDevice::get(), &poolCreateInfo, nullptr, &descriptorPool));
	}
	void VulkanRenderDevice::onResize(uint32_t width, uint32_t height)
	{
		if (width == 0 || height == 0)
			return;

		VKHelper::validateResolution(width, height);

		std::static_pointer_cast<VulkanSwapChain>(Application::getGraphicsContext()->getSwapChain())->onResize(width, height);
	}
	void VulkanRenderDevice::drawSplashScreen(const std::shared_ptr<Texture>& texture)
	{
	}
	/// <summary>
	/// end and submit
	/// </summary>
	void VulkanRenderDevice::presentInternal()
	{
		auto swapChain = std::static_pointer_cast<VulkanSwapChain>( Application::getGraphicsContext()->getSwapChain());
		swapChain ->end();		
		swapChain->queueSubmit();

		auto& frameData = swapChain->getFrameData();
		auto  semphore = frameData.commandBuffer->getSemaphore();	
		swapChain->present(semphore);
	}

	void VulkanRenderDevice::presentInternal(CommandBuffer* commandBuffer)
	{
		PLOGW("non implemented func!");
	}

	auto VulkanRenderDevice::clearRenderTarget(const std::shared_ptr<Texture>& texture, CommandBuffer* commandBuffer, const glm::vec4& clearColor) -> void
	{
		VkImageSubresourceRange subresourceRange = {};
		subresourceRange.baseMipLevel = 0;
		subresourceRange.layerCount = 1;
		subresourceRange.levelCount = 1;
		subresourceRange.baseArrayLayer = 0;

		if (texture->getType() == TextureType::Color)
		{
			auto vkTexture = (VulkanTexture2D*)texture.get();

			VkImageLayout layout = vkTexture->getImageLayout();
			vkTexture->transitionImage(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, (VulkanCommandBuffer*)commandBuffer);
			subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			VkClearColorValue clearColourValue = VkClearColorValue({ {clearColor.x, clearColor.y, clearColor.z, clearColor.w} });
			vkCmdClearColorImage(((VulkanCommandBuffer*)commandBuffer)->getCommandBuffer(), vkTexture->getImage(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, &clearColourValue, 1, &subresourceRange);
			vkTexture->transitionImage(layout, (VulkanCommandBuffer*)commandBuffer);
		}
		else if (texture->getType() == TextureType::Depth)
		{
			auto vkTexture = (VulkanTextureDepth*)texture.get();

			VkClearDepthStencilValue clearDepthStencil = { 1.0f, 0 };
			subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
			vkTexture->transitionImage(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, (VulkanCommandBuffer*)commandBuffer);
			vkCmdClearDepthStencilImage(((VulkanCommandBuffer*)commandBuffer)->getCommandBuffer(), vkTexture->getImage(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, &clearDepthStencil, 1, &subresourceRange);
		}
	}
	//temp, just clear color
	auto VulkanRenderDevice::clearRenderTarget(const glm::vec4& clearColor) -> void 
	{
		auto texture = Application::getGraphicsContext()->getSwapChain()->getCurrentImage();
		auto commandBuffer = Application::getGraphicsContext()->getSwapChain()->getCurrentCommandBuffer();
		VkImageSubresourceRange subresourceRange = {};
		subresourceRange.baseMipLevel = 0;
		subresourceRange.layerCount = 1;
		subresourceRange.levelCount = 1;
		subresourceRange.baseArrayLayer = 0;

		if (texture->getType() == TextureType::Color)
		{
			auto vkTexture = (VulkanTexture2D*)texture.get();

			VkImageLayout layout = vkTexture->getImageLayout();
			vkTexture->transitionImage(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, (VulkanCommandBuffer*)commandBuffer);
			subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			VkClearColorValue clearColourValue = VkClearColorValue({ {clearColor.x, clearColor.y, clearColor.z, clearColor.w} });
			vkCmdClearColorImage(((VulkanCommandBuffer*)commandBuffer)->getCommandBuffer(), vkTexture->getImage(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, &clearColourValue, 1, &subresourceRange);
			vkTexture->transitionImage(layout, (VulkanCommandBuffer*)commandBuffer);
		}
		else if (texture->getType() == TextureType::Depth)
		{
			auto vkTexture = (VulkanTextureDepth*)texture.get();

			VkClearDepthStencilValue clearDepthStencil = { 1.0f, 0 };
			subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
			vkTexture->transitionImage(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, (VulkanCommandBuffer*)commandBuffer);
			vkCmdClearDepthStencilImage(((VulkanCommandBuffer*)commandBuffer)->getCommandBuffer(), vkTexture->getImage(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, &clearDepthStencil, 1, &subresourceRange);
		}
	
	}
};