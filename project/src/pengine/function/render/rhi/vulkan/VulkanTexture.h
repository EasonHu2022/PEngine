#pragma once
#include "VKHelper.h"
#include "function/render/rhi/Texture.h"
#include <unordered_map>
#include "VulkanCommandBuffer.h"
#include "function/render/rhi/CommandBuffer.h"
#include "function/render/rhi/FrameBuffer.h"
namespace pengine
{

	class VulkanTexture
	{
	public:
		virtual auto transitionImage(VkImageLayout newLayout, const VulkanCommandBuffer* commandBuffer = nullptr) -> void = 0;
		virtual auto getImageLayout() const->VkImageLayout = 0;
		virtual auto getImage() const->VkImage = 0;

	};

	class VulkanTexture2D :public Texture2D, public VulkanTexture
	{
	public:
		VulkanTexture2D(uint32_t width, uint32_t height, const void* data, TextureParameters parameters = TextureParameters(), 
			TextureLoadOptions loadOptions = TextureLoadOptions());
		VulkanTexture2D(const std::string& name, const std::string& filename, TextureParameters parameters = TextureParameters(),
			TextureLoadOptions loadOptions = TextureLoadOptions());
		VulkanTexture2D(VkImage image, VkImageView imageView, VkFormat format, uint32_t width, uint32_t height);
		VulkanTexture2D();
		~VulkanTexture2D() noexcept;

		void update(int32_t x, int32_t y, int32_t w, int32_t h, const void* buffer) override;
		inline void bind(uint32_t slot = 0) const override {};
		inline void unbind(uint32_t slot = 0) const override {};
		inline void setData(const void* pixels) override {};

		inline void* getHandle() const override
		{
			return (void*)this;
		};
		inline auto getWidth() const -> uint32_t override
		{
			return width;
		}
		inline auto getHeight() const -> uint32_t override
		{
			return height;
		}

		inline auto getMipMapLevels() const -> uint32_t override
		{
			return mipLevels;
		}

		inline auto getFilePath() const -> const std::string & override
		{
			return fileName;
		}

		inline auto getType() const -> TextureType override
		{
			return TextureType::Color;
		}

		inline auto getFormat() const -> TextureFormat override
		{
			return parameters.format;
		}

		inline const auto getDescriptor() const
		{
			return &descriptor;
		}

		inline auto getDescriptorInfo() const -> void* override
		{
			return (void*)getDescriptor();
		}

		inline auto getImage() const -> VkImage override
		{
			return textureImage;
		}

		inline auto getImageLayout() const -> VkImageLayout override
		{
			return imageLayout;
		}

		inline auto getDeviceMemory() const
		{
			return textureImageMemory;
		}
		inline auto getImageView() const
		{
			return textureImageView;
		}
		inline auto getSampler() const
		{
			return textureSampler;
		}

		inline auto getVkFormat() const
		{
			return vkFormat;
		}

		auto load() -> bool;
		void updateDescriptor() ;
		void buildTexture(TextureFormat internalformat, uint32_t width, uint32_t height, bool srgb, bool depth, bool samplerShadow, bool mipmap, bool image, uint32_t accessFlag)  override;
		void transitionImage(VkImageLayout newLayout, const VulkanCommandBuffer* commandBuffer = nullptr)  override;

		VkImageView getMipImageView(uint32_t mip);

	private:
		void createSampler() ;
		void deleteSampler() ;

		std::string fileName;

		VkFormat vkFormat = VK_FORMAT_R8G8B8A8_UNORM;

		uint32_t handle = 0;
		uint32_t width = 0;
		uint32_t height = 0;
		uint32_t mipLevels = 1;
		uint32_t layerCount = 1;

		bool deleteImage = false;

		const uint8_t* data = nullptr;

		TextureParameters  parameters;
		TextureLoadOptions loadOptions;

		VkImage               textureImage = VK_NULL_HANDLE;
		VkImageView           textureImageView = VK_NULL_HANDLE;
		VkDeviceMemory        textureImageMemory = VK_NULL_HANDLE;
		VkSampler             textureSampler = VK_NULL_HANDLE;
		VkImageLayout         imageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		VkDescriptorImageInfo descriptor{};

		std::unordered_map<uint32_t, VkImageView> mipImageViews;
		VmaAllocation allocation{};
	};

	class VulkanTextureDepth : public TextureDepth, public VulkanTexture
	{
	public:
		VulkanTextureDepth(uint32_t width, uint32_t height, bool stencil);
		~VulkanTextureDepth();

		auto bind(uint32_t slot = 0) const -> void override {};
		auto unbind(uint32_t slot = 0) const -> void override {};
		auto resize(uint32_t width, uint32_t height, CommandBuffer* commandBuffer) -> void override;

		auto transitionImage(VkImageLayout newLayout, const VulkanCommandBuffer* commandBuffer = nullptr) -> void override;

		inline auto getDescriptorInfo() const -> void* override
		{
			return (void*)&descriptor;
		}
		virtual auto getHandle() const -> void*
		{
			return (void*)this;
		}

		inline auto getFilePath() const -> const std::string & override
		{
			return name;
		}

		inline auto getFormat() const -> TextureFormat override
		{
			return format;
		}

		inline auto getImage() const -> VkImage override
		{
			return textureImage;
		}

		inline auto getImageLayout() const -> VkImageLayout override
		{
			return imageLayout;
		}

		inline const auto getDeviceMemory() const
		{
			return textureImageMemory;
		}
		inline const auto getImageView() const
		{
			return textureImageView;
		}
		inline const auto getSampler() const
		{
			return textureSampler;
		}

		inline const auto getDescriptor() const
		{
			return &descriptor;
		}

		inline auto getWidth() const -> uint32_t override
		{
			return width;
		}
		inline auto getHeight() const -> uint32_t override
		{
			return height;
		}
		auto updateDescriptor() -> void;

		inline auto getVkFormat() const
		{
			return vkFormat;
		}

	protected:
		auto init(CommandBuffer* commandBuffer = nullptr) -> void;
		auto release() -> void;

	private:
		bool                  stencil = false;
		uint32_t              width = 0;
		uint32_t              height = 0;
		uint32_t              handle{};
		TextureFormat         format = TextureFormat::DEPTH;
		VkFormat              vkFormat;
		VkImageLayout         imageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		VkImage               textureImage{};
		VkDeviceMemory        textureImageMemory{};
		VkImageView           textureImageView{};
		VkSampler             textureSampler{};
		VkDescriptorImageInfo descriptor{};
		VmaAllocation allocation{};

	};

	class VulkanTextureCube : public TextureCube, public VulkanTexture
	{
	public:
		VulkanTextureCube(uint32_t size, TextureFormat format = TextureFormat::RGBA8, int32_t numMips = 1);
		VulkanTextureCube(const std::string& filePath);
		VulkanTextureCube(const std::array<std::string, 6>& files);
		VulkanTextureCube(const std::vector<std::string>& files, uint32_t mips, const TextureParameters& params, const TextureLoadOptions& loadOptions, const InputFormat& format);

		~VulkanTextureCube();
		auto bind(uint32_t slot = 0) const -> void override {};
		auto unbind(uint32_t slot = 0) const -> void override {};

		inline auto getHandle() const -> void* override
		{
			return (void*)this;
		}

		auto generateMipmap(const CommandBuffer* commandBuffer) -> void override;

		auto updateDescriptor() -> void;
		auto load(uint32_t mips) -> void;

		auto update(CommandBuffer* commandBuffer, FrameBuffer* framebuffer, int32_t cubeIndex, int32_t mipmapLevel = 0) -> void override;

		auto transitionImage(VkImageLayout newLayout, const VulkanCommandBuffer* commandBuffer = nullptr) -> void override;

		inline auto getImage() const -> VkImage override
		{
			return textureImage;
		}

		inline auto getImageLayout() const -> VkImageLayout override
		{
			return imageLayout;
		}

		inline auto getDeviceMemory() const
		{
			return textureImageMemory;
		}
		inline auto getImageView() const
		{
			return textureImageView;
		}
		inline auto getSampler() const
		{
			return textureSampler;
		}
		inline auto getWidth() const -> uint32_t override
		{
			return width;
		}
		inline auto getHeight() const -> uint32_t override
		{
			return height;
		}
		inline auto getFilePath() const -> const std::string & override
		{
			return name;
		}

		inline auto getMipMapLevels() const -> uint32_t override
		{
			return numMips;
		}
		inline auto getType() const -> TextureType override
		{
			return TextureType::Cube;
		}

		inline auto getFormat() const -> TextureFormat override
		{
			return parameters.format;
		}

		inline auto getSize() const -> uint32_t override
		{
			return size;
		}

		inline auto getDescriptorInfo() const -> void* override
		{
			return (void*)&descriptor;
		}

		inline auto getVkFormat() const
		{
			return vkFormat;
		}

	private:
		auto init() -> void;

		VkFormat    vkFormat = VK_FORMAT_R8G8B8A8_UNORM;
		std::string name;
		std::string files[6];

		uint32_t handle = 0;

		uint32_t width = 0;
		uint32_t height = 0;
		uint32_t size = 0;
		uint32_t numMips = 0;

		TextureParameters  parameters;
		TextureLoadOptions loadOptions;

		std::vector<uint8_t> data;

		VkImage               textureImage = VK_NULL_HANDLE;
		VkImageLayout         imageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		VkDeviceMemory        textureImageMemory = VK_NULL_HANDLE;
		VkImageView           textureImageView = VK_NULL_HANDLE;
		VkSampler             textureSampler;
		VkDescriptorImageInfo descriptor;

		bool deleteImg = true;
		VmaAllocation allocation{};

	};

	class VulkanTextureDepthArray : public TextureDepthArray, public VulkanTexture
	{
	public:
		VulkanTextureDepthArray(uint32_t width, uint32_t height, uint32_t count);
		~VulkanTextureDepthArray();

		auto bind(uint32_t slot = 0) const -> void override {};
		auto unbind(uint32_t slot = 0) const -> void override {};
		auto resize(uint32_t width, uint32_t height, uint32_t count) -> void override;

		auto getHandle() const -> void* override
		{
			return (void*)this;
		}

		inline auto getImageView(int32_t index) const
		{
			return imageViews[index];
		}
		inline auto getSampler() const
		{
			return textureSampler;
		}
		inline auto getDescriptor() const
		{
			return &descriptor;
		}

		inline auto getDescriptorInfo() const -> void* override
		{
			return (void*)&descriptor;
		}

		inline auto getWidth() const -> uint32_t override
		{
			return width;
		}
		inline auto getHeight() const -> uint32_t override
		{
			return height;
		}
		inline auto getFilePath() const -> const std::string & override
		{
			return name;
		}

		inline auto getType() const -> TextureType override
		{
			return TextureType::DepthArray;
		}

		inline auto getFormat() const -> TextureFormat override
		{
			return format;
		}

		inline auto getCount() const
		{
			return count;
		}

		inline auto getImage() const -> VkImage override
		{
			return textureImage;
		}

		inline auto getImageLayout() const -> VkImageLayout override
		{
			return imageLayout;
		}

		auto getHandleArray(uint32_t index) -> void* override;
		auto updateDescriptor() -> void;

		auto transitionImage(VkImageLayout newLayout, const VulkanCommandBuffer* commandBuffer) -> void override;

	protected:
		auto init() -> void override;
		auto release() -> void;

	private:
		uint32_t      handle{};
		uint32_t      width = 0;
		uint32_t      height = 0;
		uint32_t      count = 0;
		TextureFormat format;

		VkImageLayout            imageLayout;
		VkImage                  textureImage{};
		VkDeviceMemory           textureImageMemory{};
		VkImageView              textureImageView{};
		VkSampler                textureSampler{};
		VkDescriptorImageInfo    descriptor{};
		std::vector<VkImageView> imageViews;
		VmaAllocation allocation{};
	};

	class VulkanTexture3D : public Texture3D, public VulkanTexture
	{
	public:
		VulkanTexture3D(uint32_t width, uint32_t height, uint32_t depth, TextureParameters parameters, TextureLoadOptions loadOptions);
		~VulkanTexture3D();
		auto bind(uint32_t slot = 0) const -> void override {};
		auto unbind(uint32_t slot = 0) const -> void override {};
		
		auto getHandle() const -> void* override
		{
			return (void*)this;
		}

		inline auto getImageView(int32_t index) const
		{
			return imageViews[index];
		}
		inline auto getSampler() const
		{
			return textureSampler;
		}
		inline auto getDescriptor() const
		{
			return &descriptor;
		}

		inline auto getDescriptorInfo() const -> void* override
		{
			return (void*)&descriptor;
		}

		inline auto getWidth() const -> uint32_t override
		{
			return width;
		}
		inline auto getHeight() const -> uint32_t override
		{
			return height;
		}
		inline auto getFilePath() const -> const std::string & override
		{
			return name;
		}

		inline auto getType() const -> TextureType override
		{
			return TextureType::DepthArray;
		}

		inline auto getFormat() const -> TextureFormat override
		{
			return format;
		}

		inline auto getCount() const
		{
			return count;
		}

		inline auto getImage() const -> VkImage override
		{
			return textureImage;
		}

		inline auto getImageLayout() const -> VkImageLayout override
		{
			return imageLayout;
		}
		auto updateDescriptor() -> void {};
		auto transitionImage(VkImageLayout newLayout, const VulkanCommandBuffer* commandBuffer) -> void override {};
		virtual auto generateMipmaps() -> void {};
	protected:
		auto init() -> void override;
		auto release() -> void;
	private:
		uint32_t      handle{};
		uint32_t      width = 0;
		uint32_t      height = 0;
		uint32_t      count = 0;
		TextureFormat format;

		VkImageLayout            imageLayout;
		VkImage                  textureImage{};
		VkDeviceMemory           textureImageMemory{};
		VkImageView              textureImageView{};
		VkSampler                textureSampler{};
		VkDescriptorImageInfo    descriptor{};
		std::vector<VkImageView> imageViews;
		VmaAllocation allocation{};
	};
};