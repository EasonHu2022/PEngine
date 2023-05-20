#include "Application.h"
#include "function/engine/Timestep.h"
#include "core/log/PLog.h"

namespace pengine
{
	Application::Application()
	{
		/*############# platform related ######################*/
		window										= Window::create(WindowData{ 1280, 720, false, "PEngine" });
		graphicsContext								= GraphicsContext::create();
		renderDevice								= RenderDevice::create();

		/*############# platform independent ######################*/
		systemManager								= std::make_unique<SystemManager>();
	}

	void Application::init()
	{
		PLogger::init();
		window->init();
		graphicsContext->init();
		renderDevice->init();
		timer.start();


		/*########### systems register and init ##############*/
		imGuiSystem = systemManager->addSystem<ImGuiSystem>(false);
		systemManager->onInit();
	}

	int32_t Application::start()
	{
		double lastFrameTime = 0;
		init();
		//main editor loop
		while (1)
		{
			//                              reset input();
			//Input::getInput()->resetPressed();
			 
			
			//                              set timestep
			Timestep timestep = timer.stop() / 1000000.f;
			
			//push paint loop
			imGuiSystem->newFrame(timestep);
			{
				onUpdate(timestep);

				renderDevice->begin();

				onRender();
				imGuiSystem->onRender();

				renderDevice->present();     

				window->swapBuffers();
				frames++;
			}
			
			//								reset graphicsContext
			graphicsContext->clearUnused();
			lastFrameTime += timestep;
			if (lastFrameTime - secondTimer > 1.0f)        //tick later
			{
				secondTimer += 1.0f;
				//PLOGI("frames : {0}", frames);
				frames = 0;
				updates = 0;
			}
		}
	}

	void Application::onUpdate(const float &delta)
	{
		
		systemManager->onImGui();

		onImGui();

		systemManager->onUpdate(delta,scene);

		window->onUpdate();
	}

	void Application::onRender()
	{
		
	}

	void Application::onImGui()
	{
		systemManager->onImGui();
	}

	void Application::onWindowResized(uint32_t w, uint32_t h)
	{
		if (w == 0 || h == 0)
		{
			return;
		}
		graphicsContext->waitIdle();
		renderDevice->onResize(w, h);
		imGuiSystem->onResize(w, h);
		
		graphicsContext->waitIdle();
	}

	void Application::onRenderDebug()
	{
	}


	pengine::Application* Application::app = nullptr;
}