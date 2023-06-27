#include "Application.h"
#include "function/engine/Timestep.h"
#include "core/log/PLog.h"
#include "function/sceneManagement/SceneManager.h"

namespace pengine
{
	Application::Application()
	{
		//init very first to ensure log
		PLogger::init();

		/*############# platform related ######################*/
		window										= Window::create(WindowData{ 1280, 720, false, "PEngine" });
		graphicsContext								= GraphicsContext::create();
		renderDevice								= RenderDevice::create();
		
		/*############# platform independent ######################*/
		cache										= std::make_shared<Cache>();
		loaderFactory								= std::make_shared<ModelLoaderFactory>();
		systemManager								= std::make_unique<SystemManager>();
		sceneManager								= std::make_unique<SceneManager>();
	}

	void Application::init()
	{
		
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

		/// test scope
		sceneManager->doTest();


		//main editor loop
		while (1)
		{			
			//                              set timestep
			Timestep timestep = timer.stop() / 1000000.f;
			
			//push paint loop
			imGuiSystem->newFrame(timestep);
			{
				onUpdate(timestep);

				renderDevice->begin();

				onRender();

				imGuiSystem->onRender();

				//end command and submit and swapchain present
				renderDevice->present();     

				window->swapBuffers();
				//TODO ： 记一下： 按理说clear rendertarget应该和pipeline绑定，可是当没有加任何pipeline的时候，imgui就不会clear，这里先加一下
				
				frames++;
			}
			
			//								reset graphicsContext
			graphicsContext->clearUnused();
			lastFrameTime += timestep;
			if (lastFrameTime - secondTimer > 1.0f)        //tick later
			{
				secondTimer += 1.0f;
				frames = 0;
				updates = 0;
			}
		}
	}

	void Application::onUpdate(const float &delta)
	{
		//very first handle window event
		window->onUpdate();

		systemManager->onImGui();

		onImGui();

		sceneManager->onUpdate();

		systemManager->onUpdate(delta, sceneManager->getCurrentScene());	

		sceneManager->onLateUpdate();

	}

	void Application::onRender()
	{
		sceneManager->onRender();
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
		sceneManager->onResize(w, h);
		graphicsContext->waitIdle();
	}

	void Application::onRenderDebug()
	{
	}


	pengine::Application* Application::app = nullptr;
}