#include "VulkanFence.h"
#include "VulkanDevice.h"
namespace pengine
{
	VulkanFence::VulkanFence(bool createSignaled)
		: signaled(createSignaled)
	{
		VkFenceCreateInfo fenceCreateInfo{};
		fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceCreateInfo.flags = createSignaled ? VK_FENCE_CREATE_SIGNALED_BIT : 0;

		vkCreateFence(*VulkanDevice::get(), &fenceCreateInfo, nullptr, &fence);
	}
	VulkanFence::~VulkanFence()
	{
		vkDestroyFence(*VulkanDevice::get(), fence, nullptr);
	}
	void VulkanFence::reset()
	{
		if (signaled)
			VK_CHECK_RESULT(vkResetFences(*VulkanDevice::get(), 1, &fence));
		signaled = false;
	}
	bool VulkanFence::wait()
	{
		PENGINE_ASSERT(!signaled, "Fence Signaled");

		const VkResult result = vkWaitForFences(*VulkanDevice::get(), 1, &fence, true, UINT32_MAX);

		VK_CHECK_RESULT(result);
		if (result == VK_SUCCESS)
		{
			signaled = true;
			return false;
		}
		return true;
	}
	void VulkanFence::waitAndReset()
	{
		if (!isSignaled())
			wait();
		reset();
	}
	bool VulkanFence::checkState()
	{
		PENGINE_ASSERT(!signaled, "Fence Signaled");

		const VkResult result = vkGetFenceStatus(*VulkanDevice::get(), fence);
		if (result == VK_SUCCESS)
		{
			signaled = true;
			return true;
		}

		return false;
	}
	bool VulkanFence::isSignaled()
	{
		if (signaled)
		{
			return true;
		}
		else
		{
			return checkState();
		}
	}
};