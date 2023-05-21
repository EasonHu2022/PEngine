#pragma once
#include "EditorView.h"
namespace peditor
{
	class SceneView : public EditorView
	{
	public:
		SceneView(char* _name, PEngineGUI::PEngineGUIWindowFlags _flags = 0);
		auto init() -> void  override;
		auto onAdd() -> void override;
		auto onRemove() -> void override;
	private:
		auto onImGui() -> void override;
	};

	
};
