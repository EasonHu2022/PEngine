#pragma once
#include "function/render/rhi/imgui/ImGuiRenderer.h"
#include "function/ecs/system/ISystem.h"
#include "function/engine/Timestep.h"
namespace pengine
{
	
	class ImGuiSystem : public ISystem
	{
	public:
		ImGuiSystem(bool clearScreen = false);
		~ImGuiSystem();
		auto newFrame(const Timestep& step) -> void;
		auto onInit() -> void override;
		auto onUpdate(float dt, Scene* scene) -> void override;

		
		auto onImGui() -> void override {};

		auto onRender() -> void;
		auto onResize(uint32_t w, uint32_t h) -> void;
		auto setTheme() -> void  {};
	private:
		bool clearScreen;
		std::shared_ptr<ImGuiRenderer> imguiRender;
	};


};