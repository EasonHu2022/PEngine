project "Editor"
	language "C++"
	cppdialect "c++17"
	architecture "x64"
	kind "ConsoleApp"
	staticruntime "off"

	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

	includedirs
	{
		"%{wks.location}/project/src/pengine",
		thirdparty .. "/spdlog/include",
		thirdparty .. "/GLFW/include",
		thirdparty .. "/ImGui",
		thirdparty .. "/glm/include",
		thirdparty .. "/entt/include",
	}


	files
	{
		"src/**.h",
		"src/**.cpp",

		"**.lua",
	}

	links
	{
		"pengine",

	}
	


	filter "system:windows"
		system "Windows"
		systemversion "latest"
		staticruntime "Off"
		defines
		{
			"_UNICODE",
            "UNICODE",
			"_CONSOLE",
			"GLFW_INCLUDE_NONE",
			"PEGNINE_WINDOWS",
			"PEGNINE_Editor",
		}


	filter "configurations:Debug"
		defines "TO_DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "TO_RELEASE"
		runtime "Release"
		optimize "on"