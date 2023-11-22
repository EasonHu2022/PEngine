#include "WindowWin.h"
#include "core/log/PLog.h"
#include "function/engine/Input.h"
#include "Application.h"
#ifdef PENGINE_VULKAN// PENGINE_VULKAN
#	define GLFW_INCLUDE_VULKAN
#elif PENGINE_OPENGL
//gl
#elif PENGINE_DX11
//dx11
#elif PENGINE_DX12
//dx12
#endif
#include "GLFW/glfw3.h"

#ifdef PEGNINE_WINDOWS
#	define GLFW_EXPOSE_NATIVE_WIN32
#	include <GLFW/glfw3native.h>
#endif // PEGNINE_WINDOWS

namespace pengine
{
	WindowWin::WindowWin(const WindowData& _data) : data(_data)
	{
	
	}

	void WindowWin::onUpdate()
	{
		glfwPollEvents();
	}


	void WindowWin::setVSync(bool sync)
	{
#ifndef PENGINE_VULKAN
		glfwSwapInterval(sync ? 1 : 0);
#endif
	}


	void WindowWin::init()
	{
	
		if (!glfwInit())
		{
			PLOGE("failed to init glfw!");
			return;
		}
#ifdef PENGINE_VULKAN
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
#endif // PENGINE_VULKAN
		glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
		glfwWindowHint(GLFW_SAMPLES, 4);
#ifdef PENGINE_OPENGL
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#endif // PENGINE_OPENGL
		GLFWmonitor* monitor = glfwGetPrimaryMonitor();
		handle = glfwCreateWindow(data.width,data.height,data.title.c_str(),nullptr,nullptr);
		if (!handle)
		{
			PLOGE("failed to create glfw window!");
			return;
		}
		setTitle(data.title);
		glfwMakeContextCurrent(handle);
		registerNativeEvent(data);
#ifdef PENGINE_OPENGL
		if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)))
		{
			PLOGE("failed to load OpenGL Library!");
			return;
		}
#endif // PENGINE_OPENGL
		glfwSetInputMode(handle, GLFW_STICKY_KEYS, 1);
		glfwSetWindowUserPointer(handle, this);
		//register event

	}

	void WindowWin::setTitle(const std::string& title)
	{
#ifdef PENGINE_VULKAN// PENGINE_VULKAN
		auto t = title + " (Vulkan)";
#elif PENGINE_OPENGL
		auto t = title + "(OpenGL)";
		//gl
#elif PENGINE_DX11
		auto t = title + "(DX11)";
		//dx11
#elif PENGINE_DX12
		auto t = title + "(DX12)";
		//dx12
#else
		auto t = title;
#endif
		
		glfwSetWindowTitle(handle, t.c_str());
	}

	void*  WindowWin::getNativeWindow()
	{
		return glfwGetWin32Window(handle);
	}

	auto WindowWin::swapBuffers() -> void
	{
#ifdef PENGINE_OPENGL
		glfwSwapBuffers(nativeInterface);
#endif        // PENGINE_OPENGL
	}

	auto WindowWin::registerNativeEvent(const WindowData& data) -> void
	{
		glfwSetWindowSizeCallback(handle, [](GLFWwindow* win, int32_t w, int32_t h) {
			Application::get()->onWindowResized(w, h);
			});
		glfwSetWindowIconifyCallback(handle, [](GLFWwindow* win, int minimized) {
			Application::get()->onWindowIconified(minimized);
			});

		glfwSetWindowCloseCallback(handle, [](GLFWwindow* win) {
			Application::get()->onWindowClosed();
			});

		glfwSetMouseButtonCallback(handle, [](GLFWwindow* window, int32_t btnId, int32_t state, int32_t mods) {
			auto w = (WindowWin*)glfwGetWindowUserPointer(window);

			double x;
			double y;
			glfwGetCursorPos(window, &x, &y);

			if (state == GLFW_PRESS )
			{
				Input::set_mouse_clicked(btnId);
			}
			if (state == GLFW_RELEASE)
			{
				Input::set_mouse_released(btnId);
			}
			});

		glfwSetCursorPosCallback(handle, [](GLFWwindow* window, double x, double y) {
			Input::set_mouse_move(x, y);
			});

		glfwSetKeyCallback(handle, [](GLFWwindow*, int32_t key, int32_t scancode, int32_t action, int32_t mods) {
			switch (action)
			{
			case GLFW_PRESS: {
				Input::set_key_pressed(key);
				break;
			}
			case GLFW_RELEASE: {
				Input::set_key_released(key);
				break;
			}
			}
			});
		glfwSetScrollCallback(handle, [](GLFWwindow* win, double xOffset, double yOffset) {
			Input::set_mouse_scroll(xOffset,yOffset);
			});
	}
}

