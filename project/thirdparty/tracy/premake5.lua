-- Third party projects

project( "tracy" )
	language "C"
	kind "StaticLib"
	
	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

	includedirs
	{
		"./",

	}
	
	files
	{
		"**.lua",
		thirdparty .."/tracy/TracyClient.cpp",
	}
	defines
	{
		"TRACY_ENABLE",
	}
	filter "system:windows"
		systemversion "latest"
		staticruntime "off"

	filter "configurations:Debug"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		runtime "Release"
		optimize "on"