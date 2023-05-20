#pragma once
#include "volk/volk.h"

#include <vk_mem_alloc.h>

struct VkConfig
{
	static constexpr bool StandardValidationLayer = false;
	static constexpr bool AssistanceLayer = false;
	static constexpr bool EnableValidationLayers = true;
};
class VulkanDevice;
class VulkanBuffer;
class VulkanImage;
class VulkanImageView;
class VulkanInstance;
class VulkanQueryPool;
class VulkanSurface;
class VulkanSwapChain;
class VulkanDescriptorPool;
class VulkanFence;
class VulkanRenderPass;
class VulkanFrameBuffer;
class VulkanShader;
class VulkanCommandPool;
class VulkanCommandBuffer;