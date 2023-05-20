project "pengine"
	language "C++"
	architecture "x64"
	kind "SharedLib"
	staticruntime "off"
	
	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")
	files
	{
		
		"**.h",
		"**.cpp",
			
		"**.lua",
		thirdparty .. "/glm/include/**.hpp",
		thirdparty .. "/glm/include/**.inl",
		thirdparty .. "/vulkanmemoryallocator/include/*.h",
		thirdparty .. "/vulkanmemoryallocator/src/*.cpp",
	}
	
	includedirs
	{
		".",
		thirdparty .. "/ImGui",
		thirdparty .. "/GLFW/include",
		thirdparty .. "/spdlog/include",
		thirdparty .. "/glad/include",
		thirdparty .. "/VulkanSDK/include",
		thirdparty .. "/volk/include",
		thirdparty .. "/vulkanmemoryallocator/include",
		thirdparty .. "/glm/include",
		
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
			"PEGNINE_ENGINE"
		}
		

	links
	{
		"GLFW",
		"ImGui",
		"spdlog",
		"Glad",
		"volk",
	}
	postbuildcommands
		{
			("{COPY} %{cfg.buildtarget.relpath}  %{wks.location}/bin/" .. outputdir .. "/Editor ")
		}
	

	filter "configurations:Debug"
		defines "PENGINE_GENERATE_DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "PENGINE_GENERATE_RELEASE"
		runtime "Release"
		optimize "on"	

	