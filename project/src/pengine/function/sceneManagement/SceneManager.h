#pragma once
#include <string>
#include <unordered_map>
#include <memory>
#include "core/core.h"
namespace pengine
{
	class Scene;
	class Texture;
	class PENGINE_API SceneManager
	{
	public:
		SceneManager();
		~SceneManager();
		auto onUpdate() -> void;
		auto onLateUpdate() -> void;
		auto onRender() -> void;
		auto onResize(uint32_t w, uint32_t h) -> void;
		auto loadScene(const std::string& filePath) -> bool;
		auto addScene(const std::string& name, Scene* scene) -> bool;
		auto getSceneByName(const std::string& scneName) -> Scene*;
		auto addDefaultScene(uint32_t width, uint32_t height, std::shared_ptr<Texture> renderTarget) -> void;
		inline auto getCurrentScene() const { return currentScene; }
	private:
		Scene* currentScene;
		std::string currentName;
		std::unordered_map<std::string, std::shared_ptr<Scene>> allScenes;
	};

};