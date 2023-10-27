#include "Editor.h"
#include "main.h"
#include "view/viewManager.h"
namespace peditor
{
	Editor::Editor(std::string executablePath) : Application(executablePath)
	{
		viewManager = std::make_unique<ViewManager>();
	}

	void Editor::init()
	{
		Application::init();

		viewManager->init();

	}

	void Editor::onImGui()
	{
		//later decide the order
		Application::onImGui();
		//update each editor window
		viewManager->onImGui();
	}

	void Editor::onUpdate(const float& delta)
	{
		Application::onUpdate(delta);
		viewManager->onUpdate(delta);
	}
	void Editor::onRenderDebug()
	{
		Application::onRenderDebug();
	}
	void Editor::shutdown()
	{
		viewManager->release();
		Application::shutdown();
	}
}

Application* createApplication(std::string executablePath)
{
	return new peditor::Editor(executablePath);
}