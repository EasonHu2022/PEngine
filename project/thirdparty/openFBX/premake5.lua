project "OFBX"
	language "C"
	kind "StaticLib"
	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")
	staticruntime "off"
	files
	{
		"src/**.c",
		"src/**.cpp",
		"**.lua",
	}
	includedirs
    {
		"./include"
    }

	filter "configurations:Debug"
		runtime "Debug"
		symbols "on"
	filter "configurations:Release"
		runtime "Release"
		optimize "on"