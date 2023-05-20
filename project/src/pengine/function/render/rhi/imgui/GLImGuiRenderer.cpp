#include "GLImGuiRenderer.h"
#include "Application.h"
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include "core/log/PLog.h"
namespace pengine
{
	GLImGuiRenderer::GLImGuiRenderer()
	{

	}
	GLImGuiRenderer::~GLImGuiRenderer()
	{
		ImGui_ImplOpenGL3_Shutdown();
	}
	void GLImGuiRenderer::init()
	{
		if (!ImGui_ImplGlfw_InitForOpenGL((GLFWwindow*)Application::getWindow()->getNativeInterface(), true))
		{	
			PLOGE("failed to initialise ImGui GLFW adapter");
		}
		ImGui_ImplOpenGL3_Init("#version 410");
	}

	void GLImGuiRenderer::newFrame(const float& delta)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.DeltaTime = delta * 1000.0f;
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
	}

	void GLImGuiRenderer::render(CommandBuffer* commandBuffer)
	{
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}

	void GLImGuiRenderer::onResize(uint32_t width, uint32_t height)
	{
	}

	void GLImGuiRenderer::rebuildFontTexture()
	{
		ImGui_ImplOpenGL3_DestroyFontsTexture();
		ImGui_ImplOpenGL3_CreateFontsTexture();
	}
}

