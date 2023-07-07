#pragma once
#include <stdint.h>
#include <memory>
#include "function/window/window.h"
#include "function/render/rhi/GraphicsContext.h"
#include "function/render/rhi/RenderDevice.h"
#include "function/resource/Cache.h"
#include "function/render/rhi/imgui/ImGuiRenderer.h"
#include "function/ecs/system/SystemManager.h"
#include "function/sceneManagement/SceneManager.h"
#include "function/ImGui/ImGuiSystem.h"
#include "core/Timer.h"
#include "core/core.h"
#include "function/resource/ModelLoader.h"
#include "function/engine/EngineOptions.h"

//temply 
#define ASSETS_ABSOLUTE_PATH "F:/workspace/YizhouHu/PEngine/PEngine/assets/"

namespace pengine
{
	class PENGINE_API Application
	{
	public:
		Application();
		~Application();
		int32_t start();

		virtual void init();
		virtual void onUpdate(const float &delta);
		virtual void onRender();
		virtual void onImGui();
		virtual void onWindowResized(uint32_t w, uint32_t h);
		virtual void onWindowIconified(int minimized);
		virtual void onWindowClosed();
		virtual void onRenderDebug();
		virtual void shutdown();

		static Application* app;
		inline static Application* get()
		{
			return app;
		}

		inline static auto& getWindow()
		{
			return get()->window;
		}

		inline static auto& getGraphicsContext()
		{
			return get()->graphicsContext;
		}


		inline static auto& getRenderDevice()
		{
			return get()->renderDevice;
		}

		inline static auto getCache()
		{
			return get()->cache;
		}

		inline static auto& getTimer()
		{
			return get()->timer;
		}

		inline static auto& getSceneManager()
		{
			return get()->sceneManager;
		}
		inline static auto& getimGuiSystem()
		{
			return get()->imGuiSystem;
		}
		inline static auto getLoaderFactory()
		{
			return get()->loaderFactory;
		}

		inline static auto& getEngineOptions()
		{
			return get()->engineOptions;
		}
	protected:
		std::unique_ptr<Window>				window;
		std::unique_ptr<SystemManager>      systemManager;
		std::unique_ptr<SceneManager>		sceneManager;

		
		std::shared_ptr<ImGuiSystem>		imGuiSystem;
		std::shared_ptr<Cache>              cache;
		std::shared_ptr<ModelLoaderFactory> loaderFactory;





		std::shared_ptr<GraphicsContext>    graphicsContext;
		std::shared_ptr<RenderDevice>       renderDevice;
		
		//app state
		bool								m_bshouldClose = false;
		bool								m_bMinimized = false;
		//temp
		Timer                                                            timer;
		uint64_t                                                         updates = 0;
		uint64_t                                                         frames = 0;
		float                                                            secondTimer = 0.0f;
		EngineOptions													 engineOptions{};
	};
}