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
		auto onUpdate(const Timestep& dt) -> void override;
		auto release() -> void override;
	private:
		auto handleInputForSceneCamera(float dt) -> void;
		auto onResize(float width, float height) -> void;
	private:
		auto onImGui() -> void override;
		std::shared_ptr<Texture2D> sceneRenderTarget;
		void* sceneRenderTargetImId;
		//for cameraControl
		float mouseSensitivity = 0.1f;
		float zoomDampeningFactor = 0.00001f;
		float dampeningFactor = 0.00001f;
		float rotateDampeningFactor = 0.001f;

		float zoomVelocity = 0.0f;
		glm::vec3 moveDirection = { 0,0,0 };
		glm::vec2 rotateVelocity = { 0,0 };
		float moveVelocity = 50.0f;
		//state
		bool m_bFocused = true;
		bool m_bHovered = true;
	};

	
};
