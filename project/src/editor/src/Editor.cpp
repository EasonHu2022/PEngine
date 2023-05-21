#include "Editor.h"
#include "main.h"
namespace peditor
{
	Editor::Editor() 
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
	}
	void Editor::onRenderDebug()
	{
		Application::onRenderDebug();
	}
}

Application* createApplication()
{
	return new peditor::Editor();
}