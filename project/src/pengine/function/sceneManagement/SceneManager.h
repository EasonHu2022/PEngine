#pragma once
#include <string>
#include <unordered_map>
#include <memory>
namespace pengine
{
	class Scene;
	class SceneManager
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
		inline auto getCurrentScene() const { return currentScene; }
	private:
		Scene* currentScene;
		std::unordered_map<std::string, std::shared_ptr<Scene>> allScenes;
	};

};