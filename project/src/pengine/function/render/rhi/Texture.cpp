#include "Texture.h"
/*#ifdef PENGINE_OPENGL
#	include "RHI/OpenGL/GLTexture.h"
#endif */       // PENGINE_OPENGL
#include "Application.h"

#ifdef PENGINE_VULKAN
#include "vulkan/VulkanTexture.h"
#endif  
namespace pengine
{
	auto Texture::getStrideFromFormat(TextureFormat format) -> uint8_t
	{
		switch (format)
		{
		case TextureFormat::RGB:
			return 3;
		case TextureFormat::RGBA:
			return 4;
		case TextureFormat::R8:
			return 1;
		case TextureFormat::RG8:
			return 2;
		case TextureFormat::RGB8:
			return 3;
		case TextureFormat::RGBA8:
			return 4;
		case TextureFormat::RG16F:
			return 4;
		default:
			return 0;
		}
	}

	auto Texture::bitsToTextureFormat(uint32_t bits) -> TextureFormat
	{
		switch (bits)
		{
		case 8:
			return TextureFormat::R8;
		case 16:
			return TextureFormat::RG8;
		case 24:
			return TextureFormat::RGB8;
		case 32:
			return TextureFormat::RGBA8;
		case 48:
			return TextureFormat::RGB16;
		case 64:
			return TextureFormat::RGBA16;
		default:
			PENGINE_ASSERT(false, "Unsupported image bit-depth!");
			return TextureFormat::RGB8;
		}
	}

	auto Texture::calculateMipMapCount(uint32_t width, uint32_t height) -> uint32_t
	{
		uint32_t levels = 1;
		while ((width | height) >> levels)
			levels++;
		return levels;
	}

	/*--------------------------------------Texture2D------------------------------------------ - */
	auto Texture2D::create() -> std::shared_ptr<Texture2D>
	{
#ifdef PENGINE_OPENGL
		return std::make_shared<GLTexture2D>();
#endif        // PENGINE_OPENGL
#ifdef PENGINE_VULKAN
		return std::make_shared<VulkanTexture2D>();
#endif        // PENGINE_VULKAN
	}

	auto Texture2D::create(uint32_t width, uint32_t height, void* data, TextureParameters parameters, TextureLoadOptions loadOptions) -> std::shared_ptr<Texture2D>
	{
#ifdef PENGINE_OPENGL
		return std::make_shared<GLTexture2D>(width, height, data, parameters, loadOptions);
#endif        // PENGINE_OPENGL
#ifdef PENGINE_VULKAN
		return std::make_shared<VulkanTexture2D>(width, height, data, parameters, loadOptions);
#endif        // PENGINE_VULKAN
	}
	//create from file, record into cache
	auto Texture2D::create(const std::string& name, const std::string& filePath, TextureParameters parameters, TextureLoadOptions loadOptions) -> std::shared_ptr<Texture2D>
	{
#ifdef PENGINE_OPENGL
		return Application::getCache()->emplace<GLTexture2D>(name, filePath, parameters, loadOptions);
#endif        // PENGINE_OPENGL
#ifdef PENGINE_VULKAN
		return Application::getCache()->emplace<VulkanTexture2D>(name, filePath, parameters, loadOptions);
#endif        // PENGINE_VULKAN
	}

	//return a default tex
	auto Texture2D::getDefaultTexture() -> std::shared_ptr<Texture2D>
	{
		static std::shared_ptr<Texture2D> defaultTexture = create("default", "textures/default.png");
		return defaultTexture;
	}
	/*--------------------------------------TextureDepth------------------------------------------*/
	auto TextureDepth::create(uint32_t width, uint32_t height, bool stencil) -> std::shared_ptr<TextureDepth>
	{
#ifdef PENGINE_OPENGL
		return std::make_shared<GLTextureDepth>(width, height, stencil);
#endif        // MAPLE_OPENGL
#ifdef PENGINE_VULKAN
		return std::make_shared<VulkanTextureDepth>(width, height, stencil);
#endif        // MAPLE_VULKAN
	}
	/*--------------------------------------TextureCube--------------------------------------------*/
	auto TextureCube::create(uint32_t size) -> std::shared_ptr<TextureCube>
	{
#ifdef PENGINE_OPENGL
		return std::make_shared<GLTextureCube>(size);
#endif        // MAPLE_OPENGL
#ifdef PENGINE_VULKAN
		return std::make_shared<VulkanTextureCube>(size);
#endif        // MAPLE_VULKAN
	}

	auto TextureCube::create(uint32_t size, TextureFormat format, int32_t numMips) -> std::shared_ptr<TextureCube>
	{
#ifdef PENGINE_OPENGL
		return std::make_shared<GLTextureCube>(size, format, numMips);
#endif        // MAPLE_OPENGL
#ifdef PENGINE_VULKAN
		return std::make_shared<VulkanTextureCube>(size, format, numMips);
#endif        // MAPLE_VULKAN
	}
	auto TextureCube::createFromFile(const std::string& filePath) -> std::shared_ptr<TextureCube>
	{
#ifdef PENGINE_OPENGL
		return std::make_shared<GLTextureCube>(filePath);
#endif        // MAPLE_OPENGL
#ifdef PENGINE_VULKAN
		return std::make_shared<VulkanTextureCube>(filePath);
#endif        // MAPLE_VULKAN
	}
	auto TextureCube::createFromFiles(const std::array<std::string, 6>& files) -> std::shared_ptr<TextureCube>
	{
#ifdef PENGINE_OPENGL
		return std::make_shared<GLTextureCube>(files);
#endif        // MAPLE_OPENGL
#ifdef PENGINE_VULKAN
		return std::make_shared<VulkanTextureCube>(files);
#endif        // MAPLE_VULKAN
	}
	auto TextureCube::createFromVCross(const std::vector<std::string>& files, uint32_t mips, TextureParameters params, TextureLoadOptions loadOptions, InputFormat format) -> std::shared_ptr<TextureCube>
	{
#ifdef PENGINE_OPENGL
		return std::make_shared<GLTextureCube>(files, mips, params, loadOptions, format);
#endif        // MAPLE_OPENGL
#ifdef PENGINE_VULKAN
		return std::make_shared<VulkanTextureCube>(files, mips, params, loadOptions, format);
#endif        // MAPLE_VULKAN
	}
	/*--------------------------------------TextureDepthArray--------------------------------------------*/
	auto TextureDepthArray::create(uint32_t width, uint32_t height, uint32_t count) -> std::shared_ptr<TextureDepthArray>
	{
#ifdef PENGINE_OPENGL
		return std::make_shared<GLTextureDepthArray>(width, height, count);
#endif        // MAPLE_OPENGL
#ifdef PENGINE_VULKAN
		return std::make_shared<VulkanTextureDepthArray>(width, height, count);
#endif        // MAPLE_VULKAN
	}
	/*--------------------------------------TextureDepthArray--------------------------------------------*/
	auto Texture3D::create(uint32_t width, uint32_t height, uint32_t depth, TextureParameters parameters, TextureLoadOptions loadOptions) -> std::shared_ptr<Texture3D>
	{
#ifdef PENGINE_OPENGL
		return std::make_shared <GLTexture3D>(width, height, depth, parameters, loadOptions);
#endif        // MAPLE_OPENGL
#ifdef PENGINE_VULKAN
		return std::make_shared <VulkanTexture3D>(width, height, depth, parameters, loadOptions);
#endif        // MAPLE_VULKAN
	}
};