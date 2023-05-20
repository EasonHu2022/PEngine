project "GLFW"
	language "C"
	kind "StaticLib"
	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")
	
	files
	{
		"include/GLFW/glfw3.h",
		"include/GLFW/glfw3native.h",
		"src/context.c",
		"src/init.c",
		"src/input.c",
		"src/monitor.c",
		"src/vulkan.c",
		"src/window.c",

		"**.lua",
	}
	includedirs
    {
       
		thirdparty .. "/VulkanSDK/include"
    }
	filter "system:windows"
		systemversion "latest"
		staticruntime "On"

		files
		{
			"src/win32_init.c",
			"src/win32_joystick.c",
			"src/win32_monitor.c",
			"src/win32_time.c",
			"src/win32_thread.c",
			"src/win32_window.c",
			"src/wgl_context.c",
			"src/egl_context.c",
			"src/osmesa_context.c",

			"**.lua",
		}

		defines 
		{ 
			"_GLFW_WIN32",
			
			"_CRT_SECURE_NO_WARNINGS"
		}

	filter "configurations:Debug"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		runtime "Release"
		optimize "on"