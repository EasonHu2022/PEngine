project "VulkanSDK"
	language "C"
	kind "StaticLib"

	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")
	includedirs
	{
		
		 "./include"
	}
	
	files
	{
		thirdparty .."/VulkanSDK/include/**.h",
		thirdparty .."/VulkanSDK/include/**.hpp",
	}

	filter "system:windows"
		systemversion "latest"
		staticruntime "On"

	filter "configurations:Debug"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		runtime "Release"
		optimize "on"