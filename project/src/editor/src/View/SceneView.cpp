#include "sceneView.h"
#include "Application.h"
#include "EditorUIConfig.h"
#include "function/engine/Input.h"
#include "function/sceneManagement/Scene.h"

namespace peditor
{
	SceneView::SceneView(char* _name, PEngineGUI::PEngineGUIWindowFlags _flags) : EditorView(_name,_flags)
	{
	}
	auto SceneView::init() -> void
	{
		updateSizeAndPos();
		sceneRenderTarget = Texture2D::create();
		sceneRenderTarget->setName(name);
		sceneRenderTarget->buildTexture(pengine::TextureFormat::RGBA16, size.x, size.y, false, false, false);
		Application::getSceneManager()->addDefaultScene(size.x,size.y, sceneRenderTarget);
		sceneRenderTargetImId = Application::getimGuiSystem()->getImguiRenderer()->addTexture(sceneRenderTarget.get());
	}
	auto SceneView::onAdd() -> void 
	{
		PLOGI("add eidtor view : {0}", name);
	}
	auto SceneView::onRemove() -> void 
	{
		
	}

	auto SceneView::onUpdate(const Timestep& dt) -> void
	{
		handleInputForSceneCamera(dt);
	}

	auto SceneView::updateSizeAndPos() -> void 
	{
		pos.x = VIEWMARGINCOMMON;
		pos.y = VIEWMARGINTOP;
		size.x = SCENEVIEWWIDTHDEFAULT * Application::getWindow()->getWidth();
		size.y = SCENEVIEWHEIGHTDEFAULT * Application::getWindow()->getHeight();
	}

	auto SceneView::handleInputForSceneCamera(float dt) -> void
	{
		auto [camera,transform] = Application::getSceneManager()->getCurrentScene()->getCamera();
		if (Input::get_key(P_KEY_LEFT_CONTROL))
		{
			if (Input::get_mouse(0))//translate
			{
				mouseSensitivity = 1.f;
				auto position = transform->getLocalPosition();
				position.x -= Input::mouseMotion.x * mouseSensitivity * 0.5f;
				position.y += Input::mouseMotion.y * mouseSensitivity * 0.5f;
				transform->setLocalPosition(position);
			}
			if (Input::get_mouse(1))//rotate
			{
				mouseSensitivity = 0.01f;
				rotateVelocity = rotateVelocity + Input::mouseMotion * mouseSensitivity;
			}
			glm::vec3 euler = glm::degrees(transform->getLocalOrientation());
			float pitch = euler.x - rotateVelocity.y;
			float yaw = euler.y - rotateVelocity.x;

			pitch = std::min(pitch, 89.0f);
			pitch = std::max(pitch, -89.0f);

			transform->setLocalOrientation(glm::radians(glm::vec3{ pitch, yaw, 0.0f }));
		}
		rotateVelocity = rotateVelocity * std::pow(rotateDampeningFactor, dt);
		//handle scroll
		auto offset = Input::scrollOffset.y;
		if (offset != 0.0f)
		{
			zoomVelocity -= dt * offset * 40.0f;
		}
		//eps
		if (std::abs( zoomVelocity - 0.0f) > 0.0001f )
		{
			glm::vec3 pos = transform->getLocalPosition();
			pos += transform->getForwardDirection() * zoomVelocity;
			zoomVelocity = zoomVelocity * std::pow(zoomDampeningFactor, dt);

			transform->setLocalPosition(pos);
		}
	}

	auto SceneView::onImGui() -> void	
	{
		glm::vec2 uv_min = { 0.0f, 0.0f };                 // bottom-left
		glm::vec2 uv_max = { 1.0f, 1.0f };                 // top-right
		glm::vec4 tint_col = { 1.0f, 1.0f, 1.0f, 1.0f };   // No tint
		glm::vec4 border_col = { 1.0f, 1.0f, 1.0f, 0.5f }; // 50% opaque white
		PEngineGUI::image(sceneRenderTargetImId, glm::vec2(size.x, size.y), uv_min, uv_max);
	}
};