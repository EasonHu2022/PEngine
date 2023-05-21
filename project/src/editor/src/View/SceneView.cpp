#include "sceneView.h"
namespace peditor
{
	SceneView::SceneView(char* _name, PEngineGUI::PEngineGUIWindowFlags _flags) : EditorView(_name,_flags)
	{
	}
	auto SceneView::init() -> void
	{
		
	}
	auto SceneView::onAdd() -> void 
	{
		PLOGI("add eidtor view : {0}", name);
	}
	auto SceneView::onRemove() -> void 
	{
		
	}
	auto SceneView::onImGui() -> void 
	{
		PEngineGUI::button("Test PEngine GUI ", glm::vec2(50, 50));
	}
};