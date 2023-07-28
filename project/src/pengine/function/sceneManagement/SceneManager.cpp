#include "SceneManager.h"
#include "Scene.h"
#include "core/utils/StringUtils.h"
#include "function/ecs/component/Transform.h"
#include <Application.h>
#include "function/render/rhi/Texture.h"

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
	auto SceneManager::release() -> void
	{
		allScenes.clear();
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
			camera.setFar(10000.0f); 
			camera.setNear(10);
			//camera.setOrthographic(true);
			auto w = scene->width == 0 ? 4.0f : scene->width;
			auto h = scene->height == 0 ? 3.0f : scene->height;
			camera.setAspectRatio(w / h);
			auto& transform = entity.getComponent<component::Transform>();
			transform.setLocalPosition({0.0f,1000.0f,0.0f});
			transform.setLocalOrientation(glm::radians(glm::vec3(-89.0f, 0.0f, 0.0f)));
			//test
			auto light = scene->createEntity("Light");
			light.addComponent<component::Light>();
			auto& lightTransform = light.getOrAddComponent<component::Transform>();
			lightTransform.setLocalPosition({ 1000.0f,5000.0f,0.0f });
			lightTransform.setLocalOrientation(glm::radians(glm::vec3( 90.0f,0.0f,0.0f )));
		}
		return false;
	}
	auto SceneManager::getSceneByName(const std::string& scneName) -> Scene*
	{
		return nullptr;
	}
	auto SceneManager::addDefaultScene(uint32_t width, uint32_t height, std::shared_ptr<Texture> renderTarget) -> void
	{
		currentName = "defaultScene";
		addScene(currentName, new Scene(currentName,width,height,renderTarget));
		currentScene = allScenes[currentName].get();
		std::string testpath = "models/sponza/sponza.obj";
		currentScene->addModel(ASSETS_ABSOLUTE_PATH+testpath);
	}
};