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
		assetsdir.."/shaders/**.shader",
		thirdparty .. "/glm/include/**.hpp",
		thirdparty .. "/glm/include/**.inl",
		thirdparty .. "/vulkanmemoryallocator/include/*.h",
		thirdparty .. "/vulkanmemoryallocator/src/*.cpp",
		thirdparty .. "/ImGui/**.h",
		thirdparty .. "/ImGui/**.hpp",
		thirdparty .. "/ImGui/**.cpp",
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
		thirdparty .. "/entt/include",
		thirdparty .. "/tiny_obj_loader",
		thirdparty .. "/tiny_gltf",
		thirdparty .. "/openFBX/include",
		thirdparty .. "/mio",
		thirdparty .. "/stb",
		thirdparty .. "/Json",
		thirdparty .. "/SPIRV-Cross",
		thirdparty .. "/tracy",
		thirdparty .. "/Eigen",
		thirdparty .. "/meshoptimizer/include",
		assetsdir,
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
			"PEGNINE_ENGINE",
			"TRACY_ENABLE",
		}
		
	dependson "shaders"
	links
	{
		"GLFW",
		"spdlog",
		"Glad",
		"volk",
		"OFBX",
		"spirv-cross",
		"tracy",
		"meshoptimizer"
	}
	postbuildcommands
		{
			("{COPY} %{cfg.buildtarget.relpath}  %{wks.location}/bin/" .. outputdir .. "/Editor ")
		}
	

	filter "configurations:Debug"
		defines "PENGINE_GENERATE_DEBUG"
		runtime "Debug"
		symbols "on"
		editandcontinue "Off"

	filter "configurations:Release"
		defines "PENGINE_GENERATE_RELEASE"
		runtime "Release"
		optimize "on"	

	