#include "Image.h"
#include "ImageLoader.h"
namespace pengine
{
	Image::Image(const std::string& fileName)
		:fileName(fileName)
	{
		ImageLoader::loadAsset(fileName, this);
	}
};