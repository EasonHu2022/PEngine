#include "Editor.h"
#include "Main.h"
namespace pengine
{
	Editor::Editor() 
	{
		
	}

	void Editor::init()
	{
		Application::init();



	}

	void Editor::onImGui()
	{
		//update each editor window
	}

	void Editor::onUpdate(const float& delta)
	{
		Application::onUpdate(delta);
	}
	void Editor::onRenderDebug()
	{
	}
}

pengine::Application* createApplication()
{
	return new pengine::Editor();
}