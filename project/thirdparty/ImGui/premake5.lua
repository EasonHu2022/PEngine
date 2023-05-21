project "ImGui"
    kind "StaticLib"
    language "C++"
    targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")
	architecture "x64"
    files
	{
        "**.h",
		"**.cpp",
        "**.lua",
	}

    includedirs
    {
        thirdparty .. "/GLFW/include",
		thirdparty .. "/GLAD/include",
		thirdparty .. "/VulkanSDK/include",
		thirdparty .. "/volk/include",
		thirdparty .. "/glm/include"
    }

    filter "system:windows"
		systemversion "latest"
		cppdialect "C++17"
		staticruntime "off"
	

    filter "configurations:Debug"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		runtime "Release"
		optimize "on"
