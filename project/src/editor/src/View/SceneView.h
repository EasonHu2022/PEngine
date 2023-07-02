#pragma once
#include "EditorView.h"
#include "function/render/rhi/Texture.h"
using namespace pengine;
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
		auto updateSizeAndPos() -> void override;

	private:
		auto onImGui() -> void override;
		std::shared_ptr<Texture2D> sceneRenderTarget;
	};

	
};
