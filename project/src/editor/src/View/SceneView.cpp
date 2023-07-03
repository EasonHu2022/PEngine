#include "sceneView.h"
#include "Application.h"
#include "EditorUIConfig.h"

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
	}
	auto SceneView::onAdd() -> void 
	{
		PLOGI("add eidtor view : {0}", name);
	}
	auto SceneView::onRemove() -> void 
	{
		
	}
	auto SceneView::updateSizeAndPos() -> void 
	{
		pos.x = VIEWMARGINCOMMON;
		pos.y = VIEWMARGINTOP;
		size.x = SCENEVIEWWIDTHDEFAULT * Application::getWindow()->getWidth();
		size.y = SCENEVIEWHEIGHTDEFAULT * Application::getWindow()->getHeight();
	}

	auto SceneView::onImGui() -> void 
	{
		glm::vec2 uv_min = { 0.0f, 0.0f };                 // bottom-left
		glm::vec2 uv_max = { 1.0f, 1.0f };                 // top-right
		glm::vec4 tint_col = { 1.0f, 1.0f, 1.0f, 1.0f };   // No tint
		glm::vec4 border_col = { 1.0f, 1.0f, 1.0f, 0.5f }; // 50% opaque white
		PEngineGUI::image(sceneRenderTarget.get(), glm::vec2(size.x, size.y), uv_min, uv_max);
	}
};