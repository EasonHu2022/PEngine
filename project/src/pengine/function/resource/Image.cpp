#include "Image.h"
namespace pengine
{
	Image::Image(const std::string& fileName)
		:fileName(fileName)
	{
		//ImageLoader::loadAsset(fileName, this);
	}
};