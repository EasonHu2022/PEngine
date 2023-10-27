#pragma once
#include "core/core.h"
namespace pengine
{
	class AssetsManager
	{
	public:
		AssetsManager(std::string executablePath);
		~AssetsManager();
		/// <summary>
		/// get the internal assets resource path of the engine
		/// </summary>
		/// <returns></returns>
		auto GetInternalAssetsPath() -> std::string;
		/// <summary>
		/// get the product(games) asstes path(set by application)
		/// </summary>
		/// <returns></returns>
		auto GetProductAssetsPath() -> std::string;
		/// <summary>
		/// set the product path (get from/or set by application)
		/// </summary>
		/// <param name="_pruductAssetsPath"></param>
		/// <returns></returns>
		auto SetProductAssetsPath(std::string _productAssetsPath) -> void;
	private:
		std::string executablePath;
		std::string internalAssetsPath;
		std::string productAssetsPath;
	};
};