#include "SceneManager.h"
#include "Scene.h"
#include "core/utils/StringUtils.h"
#include "function/ecs/component/Transform.h"
namespace pengine
{
	SceneManager::SceneManager()
	{
		currentScene = nullptr;
	}
	SceneManager::~SceneManager()
	{
	}
	auto SceneManager::onUpdate() -> void
	{
		//for scene stuffs update accept rendergraph	
		currentScene->onUpdate();
	}
	auto SceneManager::onLateUpdate() -> void
	{
		//update rendergraph at a very late stage
		currentScene->onLateUpdate();
	}
	auto SceneManager::onRender() -> void
	{
		currentScene->onRender();
	}
	auto SceneManager::onResize(uint32_t w, uint32_t h) -> void
	{
		currentScene->onResize(w,h);
	}
	auto SceneManager::loadScene(const std::string& filePath) -> bool
	{
		auto name = StringUtils::removeExtension(StringUtils::getFileName(filePath));
		auto scene = new Scene(name);
		scene->setPath(filePath);
		return addScene(name, scene);
	}
	auto SceneManager::addScene(const std::string& name, Scene* scene) -> bool
	{
		allScenes[name] = std::shared_ptr<Scene>(scene);
		auto cameras    = scene->getEntityManager()->getEntitiesWithTypes<component::Camera>();
		if (cameras.empty())
		{
			auto  entity = scene->createEntity("Main Camera");
			auto &camera = entity.addComponent<component::Camera>();
			entity.addComponent<component::Transform>();
			camera.setFov(45.f);
			camera.setFar(100);
			camera.setNear(0.01);
			camera.setAspectRatio(4 / 3.f);
		}
		return false;
	}
	auto SceneManager::getSceneByName(const std::string& scneName) -> Scene*
	{
		return nullptr;
	}
};