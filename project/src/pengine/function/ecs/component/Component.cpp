#include "Component.h"
#include "Application.h"
#include "function/sceneManagement/Scene.h"
namespace pengine
{
	namespace component
	{
		Entity Component::getEntity()
		{
			
			return { entity, Application::get()->getSceneManager()->getCurrentScene()->getRegistry() };
		}
		auto Component::setEntity(entt::entity entity) -> void
		{
			this->entity = entity;
		}
	};
};