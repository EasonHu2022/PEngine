#include "window.h"
#include "WindowWin.h"
namespace pengine
{
	auto getNativeWindow() -> void* { return nullptr; }

	auto Window::setTitle(const std::string& title)->void
	{

	}

	std::unique_ptr<Window> Window::create(const WindowData& data)
	{
		return std::make_unique<WindowWin>(data);
	}
}

