#include "Application.h"
#include "function/engine/Timestep.h"
#include "core/log/PLog.h"
#include "function/sceneManagement/SceneManager.h"
#include "function/engine/Input.h"
#include "function/profile/profiler.h"
namespace pengine
{
	Application::Application(std::string executablePath)
	{
		//init very first to ensure log
		PLogger::init();

		/*############# platform related ######################*/
		window										= Window::create(WindowData{ 1280, 720, false, "PEngine" });
		graphicsContext								= GraphicsContext::create();
		renderDevice								= RenderDevice::create();
		
		/*############# platform independent ######################*/
		assetsManager								= std::make_unique<AssetsManager>(executablePath);
		cache										= std::make_shared<Cache>();
		loaderFactory								= std::make_shared<ModelLoaderFactory>();
		systemManager								= std::make_unique<SystemManager>();
		sceneManager								= std::make_unique<SceneManager>();
	}

	Application::~Application()
	{
		
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
		//main editor loop
		while (1)
		{			
			
			//                              set timestep
			Timestep timestep = timer.stop() / 1000000.f;
			
			//push paint loop
			imGuiSystem->newFrame(timestep);
			{
				
				onUpdate(timestep);


				//window state
				
				if (m_bMinimized) 
					continue;

				renderDevice->begin();

				onRender();

				imGuiSystem->onRender();

				//Update platform viewports
				imGuiSystem->updatePlatformWindows();
				//imGuiSystem->RenderPlatformWindowsDefault();
				//end command and submit and swapchain present
				renderDevice->present();     

				window->swapBuffers();
				
				Input::reset_state();
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
			if (m_bshouldClose)
			{
				shutdown();
				break;
			}
			//PROFILE_FRAMEMARKER();
		}
		return 0;
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
	}

	void Application::onWindowResized(uint32_t w, uint32_t h)
	{
		if (w == 0 || h == 0)
		{
			return;
		
		}
		//set deletion mode to immediately
		Application::getGraphicsContext()->forceFlushDeletionQueue();

		//flush all caches
		graphicsContext->waitIdle();
		//resize
		graphicsContext->forceClearImmediately();
		renderDevice->onResize(w, h);
		imGuiSystem->onResize(w, h);
		//set deletion mode back
		Application::getGraphicsContext()->delayFlushDeletionQueue();
		graphicsContext->waitIdle();
	}

	void Application::onWindowIconified(int minimized)
	{
		m_bMinimized = minimized == 1;
	}

	void Application::onWindowClosed()
	{
		m_bshouldClose = true;
	}


	void Application::onRenderDebug()
	{
	}

	void Application::shutdown()
	{
		graphicsContext->waitIdle();
		sceneManager->release();
		systemManager->release();
		loaderFactory->release();
		cache->release();
		renderDevice->release();
		graphicsContext->release();
	}


	pengine::Application* Application::app = nullptr;
}