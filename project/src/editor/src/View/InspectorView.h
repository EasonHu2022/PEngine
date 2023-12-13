#pragma once
#include "EditorView.h"
#include "function/render/rhi/Texture.h"

using namespace pengine;
namespace peditor
{
	class InspectorView : public EditorView
	{
	public:
		InspectorView(char* _name, ImGuiWindowFlags _flags = 0);
		auto init() -> void  override;
		auto onAdd() -> void override;
		auto onRemove() -> void override;
		auto onUpdate(const Timestep& dt) -> void override;
		auto release() -> void override;
	private:
		auto onResize(float width, float height) -> void;
	private:
		auto onImGui() -> void override;
	};

	
};
