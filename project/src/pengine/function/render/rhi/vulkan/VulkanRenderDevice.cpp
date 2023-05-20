#include "VulkanRenderDevice.h"
#include "Application.h"
#include "VulkanSwapChain.h"
#include "VulkanDevice.h"
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
};