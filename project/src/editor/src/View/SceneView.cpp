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
		drawSizeAndPos();
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
		
	}
};