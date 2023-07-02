#include "VulkanRenderDevice.h"
#include "Application.h"
#include "VulkanSwapChain.h"
#include "VulkanDevice.h"
#include "VulkanTexture.h"
#include "VulkanDescriptorSet.h"
#include "VulkanPipeline.h"

#include <memory>
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

	auto VulkanRenderDevice::bindDescriptorSetsInternal(Pipeline* pipeline, CommandBuffer* commandBuffer, uint32_t dynamicOffset, const std::vector<std::shared_ptr<DescriptorSet>>& descriptorSets) -> void
	{
		uint32_t numDynamicDescriptorSets = 0;
		uint32_t numDesciptorSets = 0;
		for (auto& descriptorSet : descriptorSets)
		{
			if (descriptorSet)
			{
				auto vkDesSet = std::static_pointer_cast<VulkanDescriptorSet>(descriptorSet);
				if (vkDesSet->isDynamic())
					numDynamicDescriptorSets++;

				descriptorSetPool[numDesciptorSets] = vkDesSet->getDescriptorSet();
				numDesciptorSets++;
			}
		}
		vkCmdBindDescriptorSets(static_cast<VulkanCommandBuffer*>(commandBuffer)->getCommandBuffer(),
			VK_PIPELINE_BIND_POINT_GRAPHICS, static_cast<VulkanPipeline*>(pipeline)->getPipelineLayout(),
			0, numDesciptorSets, descriptorSetPool, numDynamicDescriptorSets, &dynamicOffset);
	}

	auto VulkanRenderDevice::drawIndexedInternal(CommandBuffer* commandBuffer, DrawType type, uint32_t count, uint32_t start) const -> void
	{
		vkCmdDrawIndexed(static_cast<VulkanCommandBuffer*>(commandBuffer)->getCommandBuffer(), count, 1, 0, 0, 0);
	}

	auto VulkanRenderDevice::copyImageInternal(CommandBuffer* commandBuffer, Texture* src, Texture* dst) const -> void
	{
		PENGINE_ASSERT(src->getWidth() == dst->getWidth() && src->getHeight() == dst->getHeight(),"Fatal: try to copy texture with different extend !");
		PENGINE_ASSERT(src->getFormat() == dst->getFormat(), "Fatal: try to copy texture with different format !");
		if (src->getType() == TextureType::Color)
		{
			if (dst->getType() == TextureType::Color)
			{

				VulkanTexture2D* srcTexture = static_cast<VulkanTexture2D*>(src);
				VulkanTexture2D* dstTexture = static_cast<VulkanTexture2D*>(dst);

				//set dst as transfer-dst
				{
					dstTexture->transitionImage(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, static_cast<VulkanCommandBuffer*>(commandBuffer));
				}
				//set the src as TRANSFER-src
				{
					srcTexture->transitionImage(VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, static_cast<VulkanCommandBuffer*>(commandBuffer));
					static_cast<VulkanCommandBuffer*>(commandBuffer);
				}
				VkImageCopy copyRegion = {};

				copyRegion.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
				copyRegion.srcSubresource.baseArrayLayer = 0;
				copyRegion.srcSubresource.mipLevel = 0;
				copyRegion.srcSubresource.layerCount = 1;
				copyRegion.srcOffset = { 0, 0, 0 };

				copyRegion.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
				copyRegion.dstSubresource.baseArrayLayer = 0;
				copyRegion.dstSubresource.mipLevel = 0;
				copyRegion.dstSubresource.layerCount = 1;
				copyRegion.dstOffset = { 0, 0, 0 };
				copyRegion.extent.width = src->getWidth();
				copyRegion.extent.height = src->getHeight();
				copyRegion.extent.depth = 1;

				// Put image copy into command buffer
				vkCmdCopyImage(
					static_cast<VulkanCommandBuffer*>(commandBuffer)->getCommandBuffer(),
					srcTexture->getImage(),
					VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
					dstTexture->getImage(),
					VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
					1,
					&copyRegion);

				//  for next copy
				dstTexture->transitionImage(VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, static_cast<VulkanCommandBuffer*>(commandBuffer));
				//  for next render
				srcTexture->transitionImage(VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, static_cast<VulkanCommandBuffer*>(commandBuffer));
			}
			else
			{
				PLOGE("different type between src : type : {0}, dst : type : {1}", src->getType(), dst->getType());
				return;
			}
		}
		else if (src->getType() == TextureType::Depth)
		{
			if (dst->getType() == TextureType::Depth)
			{

			}
			else
			{
				PLOGE("different type between src : type : {0}, dst : type : {1}", src->getType(), dst->getType());
				return;
			}
		}
		else
		{
			PLOGE("Unsupported copy type!");
			return;
		}
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