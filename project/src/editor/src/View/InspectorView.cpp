#include "InspectorView.h"
#include "Application.h"
#include "EditorUIConfig.h"
#include "function/engine/Input.h"
#include "function/sceneManagement/Scene.h"

namespace peditor
{
	InspectorView::InspectorView(char* _name, ImGuiWindowFlags _flags) : EditorView(_name,_flags)
	{
	}
	auto InspectorView::init() -> void
	{
		flags = ImGuiWindowFlags_NoScrollWithMouse |
			ImGuiWindowFlags_NoScrollbar |
			ImGuiWindowFlags_NoCollapse;

		//set init size and pos
		pos.x = VIEWMARGINCOMMON + SCENEVIEWWIDTHDEFAULT * Application::getWindow()->getWidth();
		pos.y = VIEWMARGINTOP;
		size.x = (1 - SCENEVIEWWIDTHDEFAULT) * Application::getWindow()->getWidth();
		size.y = SCENEVIEWHEIGHTDEFAULT * Application::getWindow()->getHeight();
	}
	auto InspectorView::onAdd() -> void
	{
		PLOGI("add eidtor view : {0}", name);
	}
	auto InspectorView::onRemove() -> void
	{
		
	}

	auto InspectorView::onUpdate(const Timestep& dt) -> void
	{
	
	}

	auto InspectorView::release() -> void
	{
		
	}
	auto InspectorView::onResize(float width, float height) -> void
	{
		
	}

	auto InspectorView::onImGui() -> void
	{
		
	}
};