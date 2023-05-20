#include "VulkanTexture.h"
#include "VulkanDevice.h"
#include "VKHelper.h"
#include "VulkanContext.h"
#include "VulkanBuffer.h"
#include "VulkanCommandBuffer.h"
#include "function/resource/Image.h"
#include "VulkanFrameBuffer.h"
#include <stdint.h>
#include <memory>
namespace pengine
{
	namespace
	{
		inline auto generateMipmaps(VkImage image, VkFormat imageFormat, int32_t texWidth, int32_t texHeight, uint32_t mipLevels, uint32_t faces = 1, VkCommandBuffer commandBuffer = nullptr, VkImageLayout initLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) -> void
		{
			VkFormatProperties formatProperties;
			vkGetPhysicalDeviceFormatProperties(*VulkanDevice::get()->getPhysicalDevice(), imageFormat, &formatProperties);

			if (!(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT))
			{
				PLOGE("Texture image format does not support linear blitting!");
			}

			bool singleTime = false;

			if (commandBuffer == nullptr)
			{
				commandBuffer = VKHelper::beginSingleTimeCommands();
				singleTime = true;
			}

			VkImageMemoryBarrier barrier{};
			barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
			barrier.image = image;
			barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			barrier.subresourceRange.baseArrayLayer = 0;
			barrier.subresourceRange.layerCount = 1;
			barrier.subresourceRange.levelCount = 1;

			int32_t mipWidth = texWidth;
			int32_t mipHeight = texHeight;

			for (uint32_t i = 1; i < mipLevels; i++)
			{
				for (auto face = 0; face < faces; face++)
				{
					PLOGI("Mips : {0}, face {1}", i, face);
					barrier.subresourceRange.baseMipLevel = i - 1;
					barrier.subresourceRange.baseArrayLayer = face;
					barrier.oldLayout = initLayout;
					barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
					barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
					barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

					vkCmdPipelineBarrier(commandBuffer,
						VK_PIPELINE_STAGE_TRANSFER_BIT,
						VK_PIPELINE_STAGE_TRANSFER_BIT,
						0,
						0,
						nullptr,
						0,
						nullptr,
						1,
						&barrier);

					VkImageBlit blit{};
					blit.srcOffsets[0] = { 0, 0, 0 };
					blit.srcOffsets[1] = { mipWidth, mipHeight, 1 };
					blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
					blit.srcSubresource.mipLevel = i - 1;
					blit.srcSubresource.baseArrayLayer = face;
					blit.srcSubresource.layerCount = 1;

					blit.dstOffsets[0] = { 0, 0, 0 };
					blit.dstOffsets[1] = { mipWidth > 1 ? mipWidth / 2 : 1, mipHeight > 1 ? mipHeight / 2 : 1, 1 };
					blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
					blit.dstSubresource.mipLevel = i;
					blit.dstSubresource.baseArrayLayer = face;
					blit.dstSubresource.layerCount = 1;

					vkCmdBlitImage(commandBuffer,
						image,
						VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
						image,
						VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
						1,
						&blit,
						VK_FILTER_LINEAR);

					barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
					barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
					barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
					barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

					vkCmdPipelineBarrier(commandBuffer,
						VK_PIPELINE_STAGE_TRANSFER_BIT,
						VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
						0,
						0,
						nullptr,
						0,
						nullptr,
						1,
						&barrier);
				}

				if (mipWidth > 1)
					mipWidth /= 2;
				if (mipHeight > 1)
					mipHeight /= 2;
			}

			for (auto face = 0; face < faces; face++)
			{
				barrier.subresourceRange.baseMipLevel = mipLevels - 1;
				barrier.subresourceRange.baseArrayLayer = face;
				barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
				barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
				barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
				barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

				vkCmdPipelineBarrier(commandBuffer,
					VK_PIPELINE_STAGE_TRANSFER_BIT,
					VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
					0,
					0,
					nullptr,
					0,
					nullptr,
					1,
					&barrier);
			}

			if (singleTime)
				VKHelper::endSingleTimeCommands(commandBuffer);
		}

		inline auto getFormatSize(const TextureFormat format)
		{
			switch (format)
			{
			case TextureFormat::NONE:
				return 0;
			case TextureFormat::R8:
				return 1;
			case TextureFormat::RG8:
				return 2;
			case TextureFormat::RGB8:
			case TextureFormat::RGB:
				return 3;
			case TextureFormat::RGBA8:
			case TextureFormat::RGBA:
				return 4;
			case TextureFormat::RGB16:
				return 6;
			case TextureFormat::RGBA16:
				return 8;
			case TextureFormat::RGB32:
				return 12;
			case TextureFormat::RGBA32:
				return 16;
			case TextureFormat::DEPTH:
				return 0;
			case TextureFormat::STENCIL:
				return 0;
			case TextureFormat::DEPTH_STENCIL:
				return 0;
			case TextureFormat::SCREEN:
				return 0;
			}
		}
	}
	/*#########################VulkanTexture2D######################################*/
	VulkanTexture2D::VulkanTexture2D(uint32_t width, uint32_t height, const void* data, TextureParameters parameters, TextureLoadOptions loadOptions)
		: parameters(parameters),
		loadOptions(loadOptions),
		data((const uint8_t*)data),
		width(width),
		height(height)
	{
		vkFormat = VkConverter::textureFormatToVK(parameters.format, parameters.srgb);

		buildTexture(parameters.format, width, height, parameters.srgb, false, false, loadOptions.generateMipMaps, false, 0);
		update(0, 0, width, height, data);
	}
	VulkanTexture2D::VulkanTexture2D(const std::string& name, const std::string& filename, TextureParameters parameters, TextureLoadOptions loadOptions)
		: parameters(parameters),
		loadOptions(loadOptions),
		fileName(fileName)
	{
		this->name = name;
		deleteImage = load();
		if (!deleteImage)
			return;

		createSampler();
	}
	VulkanTexture2D::VulkanTexture2D(VkImage image, VkImageView imageView, VkFormat format, uint32_t width, uint32_t height)
		:textureImage(image), textureImageView(imageView), width(width), height(height), vkFormat(format), imageLayout(VK_IMAGE_LAYOUT_UNDEFINED)
	{
		deleteImage = false;
		updateDescriptor();
	}
	VulkanTexture2D::VulkanTexture2D()
	{
		deleteImage = false;
		textureSampler = VKHelper::createTextureSampler(
			VkConverter::textureFilterToVK(parameters.magFilter),
			VkConverter::textureFilterToVK(parameters.minFilter),
			0.0f, static_cast<float>(mipLevels), true, VulkanDevice::get()->getPhysicalDevice()->getProperties().limits.maxSamplerAnisotropy,
			VkConverter::textureWrapToVK(parameters.wrap),
			VkConverter::textureWrapToVK(parameters.wrap),
			VkConverter::textureWrapToVK(parameters.wrap));

		updateDescriptor();
	}
	VulkanTexture2D::~VulkanTexture2D()
	{
		auto& deletionQueue = VulkanContext::getDeletionQueue();
		for (auto& view : mipImageViews)
		{
			if (view.second)
			{
				auto imageView = view.second;
				deletionQueue.emplace([imageView] { vkDestroyImageView(*VulkanDevice::get(), imageView, nullptr); });
			}
		}

		mipImageViews.clear();

		deleteSampler();
	}
	void VulkanTexture2D::update(int32_t x, int32_t y, int32_t w, int32_t h, const void* buffer)
	{
		auto stagingBuffer = std::make_unique<VulkanBuffer>(VK_BUFFER_USAGE_TRANSFER_SRC_BIT, w * h * getFormatSize(parameters.format), buffer);
		auto oldLayout = imageLayout;
		transitionImage(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
		VKHelper::copyBufferToImage(stagingBuffer->getVkBuffer(), textureImage, static_cast<uint32_t>(w), static_cast<uint32_t>(h), x, y);
		transitionImage(oldLayout);
	}
	auto VulkanTexture2D::load() -> bool
	{
		auto imageSize = getFormatSize(parameters.format) * width * height;
		const uint8_t* pixel = nullptr;
		std::unique_ptr<pengine::Image> image;
		if (data != nullptr)
		{
			pixel = data;

		}
		else if (fileName != "")
		{
			//load image //TODO : Loader
		}
		mipLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(width, height)))) + 1;
		//if do not need mipmaps
		if (!loadOptions.generateMipMaps)
		{
			mipLevels = 1;
		}
		VKHelper::createImage(width, height, mipLevels, VkConverter::textureFormatToVK(parameters.format, parameters.srgb),
			VK_IMAGE_TYPE_2D, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, textureImage, textureImageMemory, 1, 0, allocation);
		VKHelper::transitionImageLayout(textureImage, VkConverter::textureFormatToVK(parameters.format, parameters.srgb),
			VK_IMAGE_LAYOUT_UNDEFINED,
			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			mipLevels, 1, nullptr, false);
		auto stagingBuffer = std::make_unique<VulkanBuffer>(VK_BUFFER_USAGE_TRANSFER_SRC_BIT, imageSize, pixel);
		VKHelper::copyBufferToImage(stagingBuffer->getVkBuffer(), textureImage, static_cast<uint32_t>(width), static_cast<uint32_t>(height));
		if (loadOptions.generateMipMaps && mipLevels > 1)
			generateMipmaps(textureImage, VkConverter::textureFormatToVK(parameters.format, parameters.srgb), width, height, mipLevels);
		transitionImage(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
		return true;
	}
	auto VulkanTexture2D::updateDescriptor() -> void
	{
		descriptor.sampler = textureSampler;
		descriptor.imageView = textureImageView;
		descriptor.imageLayout = imageLayout;
	}
	void VulkanTexture2D::buildTexture(TextureFormat internalformat, uint32_t width, uint32_t height, bool srgb, bool depth, bool samplerShadow, bool mipmap, bool image, uint32_t accessFlag) 
	{
		deleteSampler();
		this->width = width;
		this->height = height;
		deleteImage = true;
		mipLevels = 1;
		constexpr uint32_t FLAGS = VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT |
			VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		vkFormat = VkConverter::textureFormatToVK(internalformat, srgb);
		parameters.format = internalformat;
		parameters.srgb = srgb;
		VKHelper::createImage(width, height, mipLevels, vkFormat, VK_IMAGE_TYPE_2D, VK_IMAGE_TILING_OPTIMAL,
			VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | 
			VK_IMAGE_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, textureImage, textureImageMemory, 1, 0, allocation);
		textureImageView = VKHelper::createImageView(textureImage, vkFormat, mipLevels, VK_IMAGE_VIEW_TYPE_2D, VK_IMAGE_ASPECT_COLOR_BIT, 1);
		textureSampler = VKHelper::createTextureSampler(VkConverter::textureFilterToVK(parameters.magFilter),VkConverter::textureFilterToVK(parameters.minFilter),
			0.0f, static_cast<float>(mipLevels), true, VulkanDevice::get()->getPhysicalDevice()->getProperties().limits.maxSamplerAnisotropy,
			VkConverter::textureWrapToVK(parameters.wrap), VkConverter::textureWrapToVK(parameters.wrap), VkConverter::textureWrapToVK(parameters.wrap)
			);
		imageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		transitionImage(VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
		updateDescriptor();
	}
	void VulkanTexture2D::transitionImage(VkImageLayout newLayout, const VulkanCommandBuffer* commandBuffer)
	{
		if (commandBuffer)
			PENGINE_ASSERT(commandBuffer->isRecording(), "must be recording!");
		if (newLayout != imageLayout)
		{
			VKHelper::transitionImageLayout(textureImage, VkConverter::textureFormatToVK(parameters.format, parameters.srgb), imageLayout, newLayout,
				mipLevels, 1, commandBuffer ? commandBuffer->getCommandBuffer() : nullptr, false);
		}
		imageLayout = newLayout;
		updateDescriptor();
	}
	VkImageView VulkanTexture2D::getMipImageView(uint32_t mip)
	{
		if (auto iter = mipImageViews.find(mip); iter == mipImageViews.end())
		{
			mipImageViews[mip] = VKHelper::createImageView(textureImage, VkConverter::textureFormatToVK(parameters.format, parameters.srgb),
				mipLevels, VK_IMAGE_VIEW_TYPE_2D, VK_IMAGE_ASPECT_COLOR_BIT, 1, 0, mip);
		}
		return mipImageViews.at(mip);
	}
	void VulkanTexture2D::createSampler()
	{
		textureImageView = VKHelper::createImageView(textureImage,
			VkConverter::textureFormatToVK(parameters.format, parameters.srgb),
			mipLevels, VK_IMAGE_VIEW_TYPE_2D, VK_IMAGE_ASPECT_COLOR_BIT, 1);

		auto phyDevice = VulkanDevice::get()->getPhysicalDevice();

		textureSampler = VKHelper::createTextureSampler(
			VkConverter::textureFilterToVK(parameters.magFilter),
			VkConverter::textureFilterToVK(parameters.minFilter), 0.0f, static_cast<float>(mipLevels), true,
			phyDevice->getProperties().limits.maxSamplerAnisotropy,
			VkConverter::textureWrapToVK(parameters.wrap),
			VkConverter::textureWrapToVK(parameters.wrap),
			VkConverter::textureWrapToVK(parameters.wrap));

		updateDescriptor();
	}
	void VulkanTexture2D::deleteSampler()
	{
		auto& deletionQueue = VulkanContext::getDeletionQueue();

		if (textureSampler)
		{
			auto sampler = textureSampler;
			deletionQueue.emplace([sampler] { vkDestroySampler(*VulkanDevice::get(), sampler, nullptr); });
		}

		if (textureImageView)
		{
			auto imageView = textureImageView;
			deletionQueue.emplace([imageView] { vkDestroyImageView(*VulkanDevice::get(), imageView, nullptr); });
		}

		if (deleteImage)
		{
			auto image = textureImage;

			auto alloc = allocation;
			deletionQueue.emplace([image, alloc] { vmaDestroyImage(VulkanDevice::get()->getAllocator(), image, alloc); });
		}
	}

	/*#########################VulkanTextureDepth######################################*/
	VulkanTextureDepth::VulkanTextureDepth(uint32_t width, uint32_t height, bool stencil) :
		stencil(stencil), width(width), height(height)
	{
		init();
	}

	VulkanTextureDepth::~VulkanTextureDepth()
	{
		release();
	}

	auto VulkanTextureDepth::release() -> void
	{
		auto& deletionQueue = VulkanContext::getDeletionQueue();

		if (textureSampler)
		{
			auto sampler = textureSampler;
			auto imageView = textureImageView;
			deletionQueue.emplace([sampler, imageView]() {
				auto device = VulkanDevice::get();
				vkDestroyImageView(*device, imageView, nullptr);
				vkDestroySampler(*device, sampler, nullptr);
				});
		}
		auto image = textureImage;
		auto alloc = allocation;
		deletionQueue.emplace([image, alloc] { vmaDestroyImage(VulkanDevice::get()->getAllocator(), image, alloc); });
	}

	auto VulkanTextureDepth::resize(uint32_t width, uint32_t height, CommandBuffer* commandBuffer) -> void
	{
		this->width = width;
		this->height = height;
		release();
		init(commandBuffer);
	}

	auto VulkanTextureDepth::updateDescriptor() -> void
	{
		descriptor.sampler = textureSampler;
		descriptor.imageView = textureImageView;
		descriptor.imageLayout = imageLayout;
	}

	auto VulkanTextureDepth::init(CommandBuffer* commandBuffer) -> void
	{
		vkFormat = VKHelper::getDepthFormat(stencil);
		format = TextureFormat::DEPTH;
		VKHelper::createImage(width, height, 1, vkFormat, VK_IMAGE_TYPE_2D, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, textureImage, textureImageMemory, 1, 0, allocation);
		textureImageView = VKHelper::createImageView(textureImage, vkFormat, 1, VK_IMAGE_VIEW_TYPE_2D, VK_IMAGE_ASPECT_DEPTH_BIT, 1);
		transitionImage(VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, (VulkanCommandBuffer*)commandBuffer);
		textureSampler = VKHelper::createTextureSampler(VK_FILTER_LINEAR, VK_FILTER_LINEAR, 0.0f, 1.0f, true,
			VulkanDevice::get()->getPhysicalDevice()->getProperties().limits.maxSamplerAnisotropy,
			VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
			VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
			VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE);
		updateDescriptor();
	}

	auto VulkanTextureDepth::transitionImage(VkImageLayout newLayout, const VulkanCommandBuffer* commandBuffer) -> void
	{
		if (commandBuffer)
			PENGINE_ASSERT(commandBuffer->isRecording(), "must recording");

		if (newLayout != imageLayout)
		{
			VKHelper::transitionImageLayout(textureImage, vkFormat, imageLayout, newLayout, 1, 1, commandBuffer ? commandBuffer->getCommandBuffer() : nullptr);
		}
		imageLayout = newLayout;
		updateDescriptor();
	}

	/*#########################VulkanTextureCube######################################*/
	VulkanTextureCube::VulkanTextureCube(const std::array<std::string, 6>& files)
	{
		PLOGE("{0} did not implement", __FUNCTION__);
	}

	VulkanTextureCube::VulkanTextureCube(const std::vector<std::string>& files, uint32_t mips, const TextureParameters& params, const TextureLoadOptions& loadOptions, const InputFormat& format)
	{
		PLOGE("{0} did not implement", __FUNCTION__);
	}

	VulkanTextureCube::VulkanTextureCube(const std::string& filePath) :
		size(0)
	{
		files[0] = filePath;
		load(1);
		updateDescriptor();
	}

	VulkanTextureCube::VulkanTextureCube(uint32_t size, TextureFormat format, int32_t numMips) :
		numMips(numMips), width(size), height(size), deleteImg(false), size(size)
	{
		parameters.format = format;
		init();
	}
	VulkanTextureCube::~VulkanTextureCube()
	{
		auto& deletionQueue = VulkanContext::getDeletionQueue();

		if (textureSampler)
		{
			auto sampler = textureSampler;
			deletionQueue.emplace([sampler] { vkDestroySampler(*VulkanDevice::get(), sampler, nullptr); });
		}

		if (textureImageView)
		{
			auto imageView = textureImageView;
			deletionQueue.emplace([imageView] { vkDestroyImageView(*VulkanDevice::get(), imageView, nullptr); });
		}

		if (deleteImg)
		{
			auto image = textureImage;
			auto alloc = allocation;
			deletionQueue.emplace([image, alloc] { vmaDestroyImage(VulkanDevice::get()->getAllocator(), image, alloc); });

		}
	}

	auto VulkanTextureCube::generateMipmap(const CommandBuffer* commandBuffer) -> void
	{
		auto vkCmd = static_cast<const VulkanCommandBuffer*>(commandBuffer);
		transitionImage(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, vkCmd);
		generateMipmaps(textureImage, vkFormat, size, size, numMips, 6, vkCmd->getCommandBuffer());
		imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		updateDescriptor();
	}

	auto VulkanTextureCube::updateDescriptor() -> void
	{
		descriptor.sampler = textureSampler;
		descriptor.imageView = textureImageView;
		descriptor.imageLayout = imageLayout;
	}

	auto VulkanTextureCube::load(uint32_t mips) -> void
	{
		PLOGE("didn't implement");
	}

	auto VulkanTextureCube::update(CommandBuffer* commandBuffer, FrameBuffer* framebuffer, int32_t cubeIndex, int32_t mipMapLevel) -> void
	{
		auto cmd = static_cast<VulkanCommandBuffer*>(commandBuffer);

		auto frameBuffer = static_cast<VulkanFrameBuffer*>(framebuffer);

		auto& info = frameBuffer->getFrameBufferInfo();
		//TODO..... avoid using dynamic_cast
		auto vkTexture = dynamic_cast<VulkanTexture*>(info.attachments[0].get());
		auto colorImage = vkTexture->getImage();
		auto oldLayout = vkTexture->getImageLayout();

		assert(colorImage != VK_NULL_HANDLE);

		//set itself as transfer-dst
		{
			transitionImage(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, cmd);
		}
		//set the attachment as TRANSFER-src
		{
			vkTexture->transitionImage(VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, cmd);
		}

		// Copy region for transfer from framebuffer to cube face
		VkImageCopy copyRegion = {};

		copyRegion.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		copyRegion.srcSubresource.baseArrayLayer = 0;
		copyRegion.srcSubresource.mipLevel = 0;
		copyRegion.srcSubresource.layerCount = 1;
		copyRegion.srcOffset = { 0, 0, 0 };

		copyRegion.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		copyRegion.dstSubresource.baseArrayLayer = cubeIndex;
		copyRegion.dstSubresource.mipLevel = mipMapLevel;
		copyRegion.dstSubresource.layerCount = 1;
		copyRegion.dstOffset = { 0, 0, 0 };

		auto mipScale = std::pow(0.5, mipMapLevel);

		copyRegion.extent.width = width * mipScale;
		copyRegion.extent.height = height * mipScale;
		copyRegion.extent.depth = 1;

		// Put image copy into command buffer
		vkCmdCopyImage(
			cmd->getCommandBuffer(),
			colorImage,
			VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
			textureImage,
			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			1,
			&copyRegion);

		// Transform framebuffer color attachment back
		vkTexture->transitionImage(VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, cmd);

		transitionImage(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, cmd);
	}

	auto VulkanTextureCube::transitionImage(VkImageLayout newLayout, const VulkanCommandBuffer* commandBuffer /*= nullptr*/) -> void
	{
		
		if (newLayout != imageLayout)
		{
			VKHelper::transitionImageLayout(textureImage, vkFormat, imageLayout, newLayout, numMips, 6, commandBuffer ? commandBuffer->getCommandBuffer() : nullptr, false);
		}
		imageLayout = newLayout;
		updateDescriptor();
	}

	auto VulkanTextureCube::init() -> void
	{

		vkFormat = VkConverter::textureFormatToVK(parameters.format);


		VKHelper::createImage(width, height, numMips, vkFormat, VK_IMAGE_TYPE_2D, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, textureImage, textureImageMemory, 6, VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT, allocation);


		/*
		VkCommandBuffer cmdBuffer = VulkanHelper::beginSingleTimeCommands();

		VkImageSubresourceRange subresourceRange = {};
		subresourceRange.aspectMask              = VK_IMAGE_ASPECT_COLOR_BIT;
		subresourceRange.baseMipLevel            = 0;
		subresourceRange.levelCount              = numMips;
		subresourceRange.layerCount              = 6;

		VulkanHelper::setImageLayout(
			cmdBuffer,
			textureImage,
			VK_IMAGE_LAYOUT_UNDEFINED,
			VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
			subresourceRange);

		VulkanHelper::endSingleTimeCommands(cmdBuffer);*/

		textureSampler = VKHelper::createTextureSampler(
			VK_FILTER_LINEAR,
			VK_FILTER_LINEAR,
			0.0f, static_cast<float>(numMips),
			true,
			VulkanDevice::get()->getPhysicalDevice()->getProperties().limits.maxSamplerAnisotropy,
			VK_SAMPLER_ADDRESS_MODE_REPEAT, VK_SAMPLER_ADDRESS_MODE_REPEAT, VK_SAMPLER_ADDRESS_MODE_REPEAT);

		textureImageView = VKHelper::createImageView(textureImage, vkFormat, numMips,
			VK_IMAGE_VIEW_TYPE_CUBE, VK_IMAGE_ASPECT_COLOR_BIT, 6);

		imageLayout = VK_IMAGE_LAYOUT_UNDEFINED;

		updateDescriptor();
	}

	/*#########################VulkanTextureDepthArray######################################*/
	VulkanTextureDepthArray::VulkanTextureDepthArray(uint32_t width, uint32_t height, uint32_t count) :
		width(width), height(height), count(count)
	{
		init();
	}

	VulkanTextureDepthArray::~VulkanTextureDepthArray()
	{
		release();
	}

	auto VulkanTextureDepthArray::resize(uint32_t width, uint32_t height, uint32_t count) -> void
	{
		this->width = width;
		this->height = height;
		this->count = count;

		release();
		init();
	}

	auto VulkanTextureDepthArray::getHandleArray(uint32_t index) -> void*
	{
		descriptor.imageView = getImageView(index);
		return (void*)&descriptor;
	}
	auto VulkanTextureDepthArray::updateDescriptor() -> void
	{
		descriptor.sampler = textureSampler;
		descriptor.imageView = textureImageView;
		descriptor.imageLayout = imageLayout;
	}
	auto VulkanTextureDepthArray::init() -> void
	{
		auto depthFormat = VKHelper::getDepthFormat();


		VKHelper::createImage(width, height, 1, depthFormat, VK_IMAGE_TYPE_2D, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, textureImage, textureImageMemory, count, 0, allocation);

		textureImageView = VKHelper::createImageView(textureImage, depthFormat, 1, VK_IMAGE_VIEW_TYPE_2D_ARRAY, VK_IMAGE_ASPECT_DEPTH_BIT, count);
		for (uint32_t i = 0; i < count; i++)
		{
			imageViews.emplace_back(VKHelper::createImageView(textureImage, depthFormat, 1, VK_IMAGE_VIEW_TYPE_2D_ARRAY, VK_IMAGE_ASPECT_DEPTH_BIT, 1, i));
		}
		format = TextureFormat::DEPTH;
		VKHelper::transitionImageLayout(textureImage, depthFormat, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, 1, count);
		imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		textureSampler = VKHelper::createTextureSampler();
		updateDescriptor();
	}

	auto VulkanTextureDepthArray::release() -> void
	{
		auto& queue = VulkanContext::getDeletionQueue();

		auto textureImageView = this->textureImageView;
		auto textureImage = this->textureImage;
		auto textureImageMemory = this->textureImageMemory;
		auto textureSampler = this->textureSampler;
		auto imageViews = this->imageViews;
		auto size = count;

		queue.emplace([textureImageView, textureSampler, imageViews, size]() {
			vkDestroyImageView(*VulkanDevice::get(), textureImageView, nullptr);

			if (textureSampler)
				vkDestroySampler(*VulkanDevice::get(), textureSampler, nullptr);

			for (uint32_t i = 0; i < size; i++)
			{
				vkDestroyImageView(*VulkanDevice::get(), imageViews[i], nullptr);
			}
			});
		auto alloc = allocation;
		queue.emplace([textureImage, alloc] { vmaDestroyImage(VulkanDevice::get()->getAllocator(), textureImage, alloc); });
	}

	auto VulkanTextureDepthArray::transitionImage(VkImageLayout newLayout, const VulkanCommandBuffer* commandBuffer) -> void
	{

		if (newLayout != imageLayout)
		{
			VKHelper::transitionImageLayout(textureImage, VKHelper::getDepthFormat(), imageLayout, newLayout, 1, count, commandBuffer ? commandBuffer->getCommandBuffer() : nullptr);
		}
		imageLayout = newLayout;
		updateDescriptor();
	}

	/*#########################VulkanTexture3D######################################*/
	VulkanTexture3D :: VulkanTexture3D(uint32_t width, uint32_t height, uint32_t depth, TextureParameters parameters, TextureLoadOptions loadOptions)
	{
		init();
	}
	VulkanTexture3D::~VulkanTexture3D() 
	{
		release();
	};

	void VulkanTexture3D::init()
	{
		PLOGE("didn't impl");
	}
	void VulkanTexture3D::release()
	{
		PLOGE("didn't impl");
	}


};