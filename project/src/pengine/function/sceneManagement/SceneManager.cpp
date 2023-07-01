#include "SceneManager.h"
#include "Scene.h"
#include "core/utils/StringUtils.h"
#include "function/ecs/component/Transform.h"
#include <Application.h>


namespace pengine
{
	SceneManager::SceneManager()
	{	
		currentScene = nullptr;
		//test
		
	}
	SceneManager::~SceneManager()
	{
	}
	auto SceneManager::onUpdate() -> void
	{
		//for scene stuffs update accept rendergraph	
		if (currentScene)
			currentScene->onUpdate();
	}
	auto SceneManager::onLateUpdate() -> void
	{
		//update rendergraph at a very late stage
		if(currentScene)
			currentScene->onLateUpdate();
	}
	auto SceneManager::onRender() -> void
	{
		if (currentScene)
			currentScene->onRender();
	}
	auto SceneManager::onResize(uint32_t w, uint32_t h) -> void
	{
		if (currentScene)
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
			camera.setFov(45.f);
			camera.setFar(10000); 
			camera.setNear(0.01);
			camera.setAspectRatio(4 / 3.f);
			auto& transform = entity.getComponent<component::Transform>();
			transform.setLocalPosition({0.0f,0.0f,3.0f});
			//test
			auto light = scene->createEntity("Light");
			light.addComponent<component::Light>();
			light.getOrAddComponent<component::Transform>().setLocalPosition({1000.0f,1000.0f,1000.0f});
		}
		return false;
	}
	auto SceneManager::getSceneByName(const std::string& scneName) -> Scene*
	{
		return nullptr;
	}
	auto SceneManager::doTest() -> void
	{
		currentName = "defaultScene";
		addScene(currentName, new Scene(currentName));
		currentScene = allScenes[currentName].get();
		std::string testpath = "models/cubes/cube.obj";
		//std::string testpath = "F:/workspace/YizhouHu/PEngine/PEngine/assets/models/cubes/cube.obj";
		currentScene->addModel(ASSETS_ABSOLUTE_PATH+testpath);
	}
};