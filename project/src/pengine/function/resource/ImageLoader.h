#pragma once
#include <string>
#include "Image.h"
namespace pengine
{
	class ImageLoader
	{
	public:
		static auto loadAsset(const std::string& name, bool mipmaps = true, bool flipY = true) -> std::unique_ptr<Image>;
		static auto loadAsset(const std::string& name, Image* image) -> void;
	};

};