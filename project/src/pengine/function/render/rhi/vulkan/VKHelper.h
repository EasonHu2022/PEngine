#pragma once
#include "VKWrapper.h"
#include "core/log/PLog.h"
#include <vector>
#include <optional>
#include <set>
#include "function/render/rhi/Defs.h"
#include "function/render/rhi/Shader.h"
#include <iostream>
#define VK_CHECK_RESULT(f)                                                                                                        \
	{                                                                                                                             \
		VkResult res = (f);                                                                                                       \
		if (res != VK_SUCCESS)                                                                                                    \
		{                                                                                                                         \
			PLOGE("Fatal : VkResult is \" {0} \" in {1} at line {2}", VKHelper::errorString(res).c_str(), __FILE__, __LINE__); \
			assert(res == VK_SUCCESS);                                                                                            \
		}                                                                                                                         \
	}

namespace pengine
{
	enum class DescriptorType;
	struct SwapChainSupportDetails
	{
		VkSurfaceCapabilitiesKHR        capabilities;
		std::vector<VkSurfaceFormatKHR> formats;
		std::vector<VkPresentModeKHR>   presentModes;
	};

	struct QueueFamilyIndices
	{
		std::optional<uint32_t> graphicsFamily;
		std::optional<uint32_t> presentFamily;
		std::optional<uint32_t> computeFamily;
		auto                    isComplete()
		{
			return graphicsFamily.has_value() && presentFamily.has_value() && computeFamily.has_value();
		}
	};

	namespace VKHelper
	{
		void validateResolution(uint32_t& width, uint32_t& height);
		VkInstanceCreateInfo instanceCreateInfo(const VkApplicationInfo& appInfo,
			const std::vector<const char*>& extensions, 
			const std::vector<const char*>& validationLayers,
			bool validationLayer);
		VkApplicationInfo getApplicationInfo(const std::string& name);
		QueueFamilyIndices findQueueFamilies(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface);
		uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
		SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface);
		bool checkDeviceExtensionSupport(VkPhysicalDevice device, const std::vector<const char*>& deviceExtensions);
		bool checkValidationLayerSupport(const std::vector<const char*>& layerNames);
		void createImage(uint32_t width, uint32_t height, uint32_t mipLevels,
			VkFormat format, VkImageType imageType, VkImageTiling tiling,
			VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, 
			VkDeviceMemory& imageMemory, uint32_t arrayLayers, VkImageCreateFlags flags, VmaAllocation& allocation);
		VkImageView createImageView(VkImage image, VkFormat format, uint32_t mipLevels, VkImageViewType viewType,
			VkImageAspectFlags aspectMask, uint32_t layerCount, uint32_t baseArrayLayer = 0, uint32_t baseMipLevel = 0);
		void createBuffer(VkBuffer& buffer, VkDeviceMemory& bufferMemory,
			VkPhysicalDevice physicalDevice, VkDevice device,
			VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBufferCreateFlags flags = 0,
			VkSharingMode sharingMode = VK_SHARING_MODE_EXCLUSIVE, const std::vector<uint32_t>& queueFamilyIndices = {});
		// Put an image memory barrier for setting an image layout on the sub resource into the given command buffer
		void setImageLayout(
			VkCommandBuffer         cmdbuffer,
			VkImage                 image,
			VkImageLayout           oldImageLayout,
			VkImageLayout           newImageLayout,
			VkImageSubresourceRange subresourceRange,
			VkPipelineStageFlags    srcStageMask = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
			VkPipelineStageFlags    dstStageMask = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT);
		// Uses a fixed sub resource layout with first mip level and layer
		void setImageLayout(
			VkCommandBuffer      cmdbuffer,
			VkImage              image,
			VkImageAspectFlags   aspectMask,
			VkImageLayout        oldImageLayout,
			VkImageLayout        newImageLayout,
			VkPipelineStageFlags srcStageMask = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
			VkPipelineStageFlags dstStageMask = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT) ;

		std::string physicalDeviceTypeString(VkPhysicalDeviceType type);
		std::string errorString(VkResult errorCode);


		auto getSupportedDepthFormat(VkPhysicalDevice physicalDevice, VkFormat* depthFormat)->VkBool32;
		auto getSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features)->VkFormat;
		auto getDepthFormat(bool stencil = false)->VkFormat;
		inline auto semaphoreCreateInfo()
		{
			VkSemaphoreCreateInfo semaphoreCreateInfo{};
			semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
			return semaphoreCreateInfo;
		}

		inline auto submitInfo()
		{
			VkSubmitInfo submitInfo{};
			submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
			return submitInfo;
		}

		inline auto bufferCreateInfo()
		{
			VkBufferCreateInfo bufCreateInfo{};
			bufCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
			return bufCreateInfo;
		}

		inline auto bufferCreateInfo(
			VkBufferUsageFlags usage,
			VkDeviceSize       size)
		{
			VkBufferCreateInfo bufCreateInfo{};
			bufCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
			bufCreateInfo.usage = usage;
			bufCreateInfo.size = size;
			return bufCreateInfo;
		}

		inline auto memoryAllocateInfo()
		{
			VkMemoryAllocateInfo memAllocInfo{};
			memAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
			return memAllocInfo;
		}

		inline auto mappedMemoryRange()
		{
			VkMappedMemoryRange mappedMemoryRange{};
			mappedMemoryRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
			return mappedMemoryRange;
		}

		inline auto imageCreateInfo()
		{
			VkImageCreateInfo imageCreateInfo{};
			imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
			return imageCreateInfo;
		}

		/** @brief Initialize an image memory barrier with no image transfer ownership */
		inline auto imageMemoryBarrier()
		{
			VkImageMemoryBarrier imageMemoryBarrier{};
			imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
			imageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			imageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			return imageMemoryBarrier;
		}

		inline auto commandBufferAllocateInfo(
			VkCommandPool        commandPool,
			VkCommandBufferLevel level,
			uint32_t             bufferCount)
		{
			VkCommandBufferAllocateInfo commandBufferAllocateInfo{};
			commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
			commandBufferAllocateInfo.commandPool = commandPool;
			commandBufferAllocateInfo.level = level;
			commandBufferAllocateInfo.commandBufferCount = bufferCount;
			return commandBufferAllocateInfo;
		}

		inline auto commandPoolCreateInfo()
		{
			VkCommandPoolCreateInfo cmdPoolCreateInfo{};
			cmdPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
			return cmdPoolCreateInfo;
		}

		inline auto commandBufferBeginInfo()
		{
			VkCommandBufferBeginInfo cmdBufferBeginInfo{};
			cmdBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			return cmdBufferBeginInfo;
		}

		inline auto renderPassCreateInfo()
		{
			VkRenderPassCreateInfo renderPassCreateInfo{};
			renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
			return renderPassCreateInfo;
		}

		inline auto fenceCreateInfo(VkFenceCreateFlags flags = 0)
		{
			VkFenceCreateInfo fenceCreateInfo{};
			fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
			fenceCreateInfo.flags = flags;
			return fenceCreateInfo;
		}

		inline auto hasStencilComponent(VkFormat format)
		{
			return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
		}

		inline VkPushConstantRange pushConstantRange(VkShaderStageFlags stageFlags, uint32_t size, uint32_t offset)
		{
			VkPushConstantRange pushConstantRange{};
			pushConstantRange.stageFlags = stageFlags;
			pushConstantRange.offset = offset;
			pushConstantRange.size = size;
			return pushConstantRange;
		}

		auto chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& formats)->VkSurfaceFormatKHR;
		auto chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& presentModes, bool vsync)->VkPresentModeKHR;
		auto chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities)->VkExtent2D;

		auto transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t mipLevels = 1, uint32_t layerCount = 1, VkCommandBuffer commandBuffer = nullptr, bool depth = true, uint32_t baseArrayLayer = 0) -> void;
		auto copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height, int32_t offsetX = 0, int32_t offsetY = 0) -> void;
		auto createTextureSampler(VkFilter magFilter = VK_FILTER_LINEAR, VkFilter minFilter = VK_FILTER_LINEAR, float minLod = 0.0f, float maxLod = 1.0f, bool anisotropyEnable = false, float maxAnisotropy = 1.0f, VkSamplerAddressMode modeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE, VkSamplerAddressMode modeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE, VkSamplerAddressMode modeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE)->VkSampler;
		auto beginSingleTimeCommands()->VkCommandBuffer;
		auto endSingleTimeCommands(VkCommandBuffer commandBuffer) -> void;

		auto createImage(const VkImageCreateInfo& imageInfo, VkImage& image, VmaAllocation& allocation) -> void;

	        // namespace VulkanHelper
	};
	namespace VkConverter
	{
		auto textureFormatToVK(const TextureFormat& format, bool srgb = true)->VkFormat;
		auto textureFilterToVK(TextureFilter filter)->VkFilter;
		auto textureWrapToVK(TextureWrap wrap)->VkSamplerAddressMode;
		auto drawTypeToTopology(DrawType type)->VkPrimitiveTopology;
		auto cullModeToVk(CullMode cull)->VkCullModeFlags;
		auto polygonModeToVk(PolygonMode mode)->VkPolygonMode;
		auto descriptorTypeToVK(DescriptorType type)->VkDescriptorType;
		auto shaderTypeToVK(const ShaderType& shaderName)->VkShaderStageFlagBits;
	};
};

