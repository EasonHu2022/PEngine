//#include "sceneView.h"
//#include "Application.h"
//#include "EditorUIConfig.h"
//#include "function/engine/Input.h"
//#include "function/sceneManagement/Scene.h"
//
//namespace peditor
//{
//	SceneView::SceneView(char* _name, PEngineGUI::PEngineGUIWindowFlags _flags) : EditorView(_name,_flags)
//	{
//	}
//	auto SceneView::init() -> void
//	{
//		flags = PEngineGUIWindowFlags_::PEngineGUIWindowFlags_NoScrollWithMouse |
//			PEngineGUIWindowFlags_::PEngineGUIWindowFlags_NoScrollbar |
//			PEngineGUIWindowFlags_::PEngineGUIWindowFlags_NoMove |
//			PEngineGUIWindowFlags_::PEngineGUIWindowFlags_NoCollapse;
//			
//		//set init size and pos
//		pos.x = VIEWMARGINCOMMON;
//		pos.y = VIEWMARGINTOP;
//		size.x = SCENEVIEWWIDTHDEFAULT * Application::getWindow()->getWidth();
//		size.y = SCENEVIEWHEIGHTDEFAULT * Application::getWindow()->getHeight();
//		sceneRenderTarget = Texture2D::create();
//		sceneRenderTarget->setName(name);
//		sceneRenderTarget->buildTexture(pengine::TextureFormat::RGBA16, size.x, size.y, false, false, false);
//		Application::getSceneManager()->addDefaultScene(size.x,size.y, sceneRenderTarget);
//		sceneRenderTargetImId = Application::getimGuiSystem()->getImguiRenderer()->addTexture(sceneRenderTarget.get());
//	}
//	auto SceneView::onAdd() -> void 
//	{
//		PLOGI("add eidtor view : {0}", name);
//	}
//	auto SceneView::onRemove() -> void 
//	{
//		
//	}
//
//	auto SceneView::onUpdate(const Timestep& dt) -> void
//	{
//		handleInputForSceneCamera(dt);
//	}
//
//	auto SceneView::release() -> void 
//	{
//		sceneRenderTarget.reset();
//
//	}
//	auto SceneView::handleInputForSceneCamera(float dt) -> void
//	{
//		if (!m_bFocused || !m_bHovered)
//			return;
//		auto [camera,transform] = Application::getSceneManager()->getCurrentScene()->getCamera();
//				
//		if (Input::get_mouse(1))//rotate
//		{
//			mouseSensitivity = 0.4f;
//			rotateVelocity = rotateVelocity + Input::mouseMotion * mouseSensitivity * dt;
//		}
//		glm::vec3 euler = glm::degrees(transform->getLocalOrientation());
//		float pitch = euler.x - rotateVelocity.y;
//		float yaw = euler.y - rotateVelocity.x;
//
//		pitch = std::min(pitch, 89.0f);
//		pitch = std::max(pitch, -89.0f);
//
//		transform->setLocalOrientation(glm::radians(glm::vec3{ pitch, yaw, 0.0f }));
//		
//		rotateVelocity = rotateVelocity * std::pow(rotateDampeningFactor, dt);
//		//handle scroll
//		auto offset = Input::scrollOffset.y;
//		if (offset != 0.0f)
//		{
//			zoomVelocity -= dt * offset * 40.0f;
//		}
//		//eps
//		if (std::abs( zoomVelocity - 0.0f) > 0.0001f )
//		{
//			glm::vec3 pos = transform->getLocalPosition();
//			pos += transform->getForwardDirection() * zoomVelocity;
//			zoomVelocity = zoomVelocity * std::pow(zoomDampeningFactor, dt);
//
//			transform->setLocalPosition(pos);
//		}
//		//handle key control
//		glm::vec3 moveDir = { 0.0f,0.0f,0.0f };
//		if (Input::get_key(P_KEY_W))//translate
//		{
//			moveDir = transform->getForwardDirection() * -1.0f;
//		}
//		if (Input::get_key(P_KEY_S))//translate
//		{
//			moveDir = transform->getForwardDirection() * 1.0f;
//		}
//		if (Input::get_key(P_KEY_A))//translate
//		{
//			moveDir = transform->getRightDirection() * -1.0f;
//		}
//		if (Input::get_key(P_KEY_D))//translate
//		{
//			moveDir = transform->getRightDirection() * 1.0f;
//		}
//		if (Input::get_key(P_KEY_Q))//translate
//		{
//			moveDir = transform->getUpDirection() * 1.0f;
//		}
//		if (Input::get_key(P_KEY_E))//translate
//		{
//			moveDir = transform->getUpDirection() * -1.0f;
//		}
//		auto position = transform->getLocalPosition();
//		position += moveDir * moveVelocity * dt;
//		transform->setLocalPosition(position);
//	}
//
//	auto SceneView::onResize(float width, float height) -> void
//	{
//		Application::getGraphicsContext()->waitIdle();
//		if (sceneRenderTargetImId)
//			Application::getimGuiSystem()->getImguiRenderer()->removeTexture(sceneRenderTargetImId);
//		sceneRenderTarget->buildTexture(pengine::TextureFormat::RGBA16, size.x, size.y, false, false, false);
//		sceneRenderTargetImId = Application::getimGuiSystem()->getImguiRenderer()->addTexture(sceneRenderTarget.get());
//		Application::getSceneManager()->getCurrentScene()->onResize(width,height);
//	}
//
//	auto SceneView::onImGui() -> void	
//	{
//		//update state
//		m_bFocused = PEngineGUI::isWindowFocused();
//		m_bHovered = PEngineGUI::isWindowHovered();
//		auto newsize = PEngineGUI::getWindowSize();
//		pos = PEngineGUI::getWindowPos();
//		if (size != newsize)
//		{
//			size = newsize;
//			//update size
//			auto w = size.x - (static_cast<int>(size.x) % 2 != 0 ? 1.0f : 0.0f);
//			auto h = size.y - (static_cast<int>(size.y) % 2 != 0 ? 1.0f : 0.0f);
//			onResize(w, h);
//		}
//		glm::vec2 uv_min = { 0.0f, 0.0f };                 // bottom-left
//		glm::vec2 uv_max = { 1.0f, 1.0f };                 // top-right
//		glm::vec4 tint_col = { 1.0f, 1.0f, 1.0f, 1.0f };   // No tint
//		glm::vec4 border_col = { 1.0f, 1.0f, 1.0f, 0.5f }; // 50% opaque white
//		PEngineGUI::image(sceneRenderTargetImId, glm::vec2(size.x, size.y), uv_min, uv_max);
//	}
//};