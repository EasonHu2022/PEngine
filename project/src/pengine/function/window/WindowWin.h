#pragma once
#include "Window.h"
struct GLFWwindow;
namespace pengine
{
	class WindowWin : public Window
	{
	public:
		WindowWin(const WindowData& data);
		virtual ~WindowWin() {};
		virtual void onUpdate() override;
		virtual void setVSync(bool sync) override;
		virtual void init() override;
		virtual void setTitle(const std::string& title) override;
		auto getNativeWindow() -> void* override;
		inline auto getNativeInterface() -> void* override
		{
			return handle;
		}
		auto swapBuffers() -> void override;

		inline bool isVSync() const override
		{
			return data.vsync;
		}

		inline uint32_t getWidth() const override
		{
			return data.width;
		};

		inline uint32_t getHeight() const override
		{
			return data.height;
		};

	private:
		auto registerNativeEvent(const WindowData& data) -> void;
		WindowData			data;
		GLFWwindow* handle = nullptr;
	};

};
