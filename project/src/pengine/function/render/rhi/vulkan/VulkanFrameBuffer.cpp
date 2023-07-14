#include "VulkanFrameBuffer.h"
#include "VulkanTexture.h"
#include "VulkanContext.h"
#include "VulkanDevice.h"
#include "VulkanRenderPass.h"
namespace pengine
{
	VulkanFrameBuffer::VulkanFrameBuffer(const FrameBufferInfo& info):
		info(info)
	{
		std::vector<VkImageView> attachments;
		for (auto& attachment : info.attachments)
		{
			switch (attachment->getType())
			{
			case TextureType::Color:
				attachments.emplace_back(static_cast<VulkanTexture2D*>(attachment.get())->getImageView());
				break;
			case TextureType::Depth:
				attachments.emplace_back(static_cast<VulkanTextureDepth*>(attachment.get())->getImageView());
				break;
			case TextureType::DepthArray:
				attachments.emplace_back(static_cast<VulkanTextureDepthArray*>(attachment.get())->getImageView(info.layer));
				break;
			case TextureType::Cube: //TODO
				attachments.emplace_back(static_cast<VulkanTextureCube *>(attachment.get())->getImageView());
				break;
			}
		}

		VkFramebufferCreateInfo framebufferInfo{};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = *std::static_pointer_cast<VulkanRenderPass>(info.renderPass);
		framebufferInfo.attachmentCount = attachments.size();
		framebufferInfo.pAttachments = attachments.data();
		framebufferInfo.width = info.width;
		framebufferInfo.height = info.height;
		framebufferInfo.layers = 1;
		framebufferInfo.pNext = nullptr;
		framebufferInfo.flags = 0;

		VK_CHECK_RESULT(vkCreateFramebuffer(*VulkanDevice::get(), &framebufferInfo, nullptr, &buffer));
	}

	VulkanFrameBuffer::~VulkanFrameBuffer()
	{
		auto& deletionQueue = VulkanContext::get()->getDeletionQueue();
		auto  framebuffer = this->buffer;
		deletionQueue.emplace([framebuffer] { vkDestroyFramebuffer(*VulkanDevice::get(), framebuffer, VK_NULL_HANDLE); });
	}
};
