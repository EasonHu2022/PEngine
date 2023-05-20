#pragma once
#include <vector>
#include <string>
#include <memory>
namespace pengine
{
	struct WindowData
	{
		uint32_t	width;
		uint32_t	height;
		bool		vsync;
		std::string	title;
	};

	class Window
	{
	public:
		Window() = default;
		virtual ~Window() {}

		virtual void onUpdate()					= 0;
		virtual uint32_t getWidth()	const		= 0;
		virtual uint32_t getHeight() const		= 0;
		virtual void setVSync(bool sync)		= 0;
		virtual bool isVSync() const			= 0;
		virtual void init()						= 0;
		virtual void setTitle(const std::string &title)					= 0;
		virtual auto getNativeInterface() -> void* = 0;
		virtual auto getNativeWindow() -> void*
		{
			return nullptr;
		}
		static std::unique_ptr<Window> create(const WindowData& data);
		virtual auto swapBuffers() -> void {};
		inline auto getScale() const
		{
			return scale;
		}
		inline auto getExtensions() const
		{
			return extensions;
		};
	protected:
		std::vector<const char*>	extensions;
		float						scale = 1.f;
	};
}