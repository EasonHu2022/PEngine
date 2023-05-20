#pragma once
#include <stdint.h>
#include <memory>
#include "function/window/window.h"
#include "function/render/rhi/GraphicsContext.h"
#include "function/render/rhi/RenderDevice.h"
#include "function/resource/Cache.h"
#include "function/render/rhi/imgui/ImGuiRenderer.h"
#include "function/ecs/system/SystemManager.h"
#include "function/ImGui/ImGuiSystem.h"
#include "core/Timer.h"
#include "core/core.h"
//后续需要做一个api选择类，这里先无脑vulkan

namespace pengine
{
	class PENGINE_API Application
	{
	public:
		Application();
		int32_t start();

		virtual void init();
		virtual void onUpdate(const float &delta);
		virtual void onRender();
		virtual void onImGui();
		virtual void onWindowResized(uint32_t w, uint32_t h);
		virtual void onRenderDebug();

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
	protected:
		std::unique_ptr<Window>				window;
		std::shared_ptr<GraphicsContext>    graphicsContext;
		std::shared_ptr<RenderDevice>       renderDevice;
		std::shared_ptr<SystemManager>      systemManager;
		std::shared_ptr<ImGuiSystem>		imGuiSystem;
		std::shared_ptr<Cache>              cache;
		//temp
		Scene* scene;
		Timer                                                            timer;
		uint64_t                                                         updates = 0;
		uint64_t                                                         frames = 0;
		float                                                            secondTimer = 0.0f;
	};
}