#include"AssetsManager.h"
namespace pengine
{
constexpr auto ASSETS_PATH_NAME = "assets/";
	AssetsManager::AssetsManager(std::string _executablePath)
	{
		executablePath = _executablePath;
		//process the executable path to get the assets paths
		{
			size_t pos = executablePath.rfind("bin");
			if (pos != std::string::npos)
			{
				internalAssetsPath = executablePath.substr(0,pos) + ASSETS_PATH_NAME;
			}
		}
	}
	AssetsManager::~AssetsManager()
	{
	}
	auto AssetsManager::GetInternalAssetsPath() -> std::string
	{
		return internalAssetsPath;
	}
	auto AssetsManager::GetProductAssetsPath() -> std::string
	{
		return productAssetsPath;
	}
	auto AssetsManager::SetProductAssetsPath(std::string _productAssetsPath) -> void
	{
		productAssetsPath = _productAssetsPath;
	}
};