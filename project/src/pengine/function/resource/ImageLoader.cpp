#include <memory>
#include <stdexcept>
#define STBI_NO_PSD
#define STBI_NO_PIC
#define STBI_NO_GIF
#define STBI_NO_PNM
#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#endif // !STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "core/utils/StringUtils.h"
#include "function/render/rhi/Defs.h"
#include "ImageLoader.h"
namespace pengine
{
	auto ImageLoader::loadAsset(const std::string& name, bool mipmaps, bool flipY) -> std::unique_ptr<Image>
	{
		bool hdr = stbi_is_hdr(name.c_str());
		PLOGI("load image : {0}", name);
		stbi_set_flip_vertically_on_load(flipY);
		int32_t       width;
		int32_t       height;
		int32_t       channels;
		TextureFormat format = hdr ? TextureFormat::RGBA32 : TextureFormat::RGBA8;
		uint8_t* data = hdr ?
			(uint8_t*)stbi_loadf(name.c_str(), &width, &height, &channels, STBI_rgb_alpha) :
			stbi_load(name.c_str(), &width, &height, &channels, STBI_rgb_alpha);
		uint32_t imageSize = width * height * 4 * (hdr ? sizeof(float) : sizeof(uint8_t));
		assert(data);
		return std::make_unique<Image>(format, width, height, data, imageSize, channels, mipmaps, hdr);
	}
	auto ImageLoader::loadAsset(const std::string& name, Image* image) -> void
	{
		bool hdr = stbi_is_hdr(name.c_str());

		stbi_set_flip_vertically_on_load(1);

		int32_t       width;
		int32_t       height;
		int32_t       channels;
		TextureFormat format = TextureFormat::RGBA8;
		uint8_t* data = stbi_load(name.c_str(), &width, &height, &channels, STBI_rgb_alpha);
		uint32_t      imageSize = width * height * 4;
		assert(data);

		image->setChannel(4);
		image->setWidth(width);
		image->setHeight(height);
		image->setPixelFormat(format);
		image->setData(data);
		image->setSize(imageSize);
	}
};