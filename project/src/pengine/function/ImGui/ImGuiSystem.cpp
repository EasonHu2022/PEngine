#include "ImGuiSystem.h"
#include "core/core.h"
#include "core/log/PLog.h"
#include "Application.h"
#include "function/render/rhi/imgui/ImGuiRenderer.h"
#include "function/render/rhi/SwapChain.h"
namespace pengine
{
	ImGuiSystem::ImGuiSystem(bool clearScreen) :
		clearScreen(clearScreen)
	{
	}
	ImGuiSystem::~ImGuiSystem()
	{
	}

	auto ImGuiSystem::newFrame(const Timestep& step) -> void
	{
		imguiRender->newFrame(step);
	}

	auto ImGuiSystem::onInit() -> void
	{
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		io.IniFilename = "";
		ImGui::StyleColorsDark();
		io.DisplaySize = ImVec2(
			static_cast<float>(Application::getWindow()->getWidth()),
			static_cast<float>(Application::getWindow()->getHeight()));
		imguiRender = ImGuiRenderer::create(
			Application::getWindow()->getWidth(),
			Application::getWindow()->getHeight(),
			clearScreen);

		io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
		imguiRender->init();
		setTheme();
	}

	auto ImGuiSystem::onUpdate(float dt, Scene* scene) -> void
	{
		ImGui::Render();
	}

	auto ImGuiSystem::onRender() -> void
	{
		imguiRender->render(Application::getGraphicsContext()->getSwapChain()->getCurrentCommandBuffer());
	}

	auto ImGuiSystem::onResize(uint32_t w, uint32_t h) -> void
	{		
		imguiRender->onResize(w, h);
	}
};