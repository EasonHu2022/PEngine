workspace "PEngine"
	language "C++"
	cppdialect "C++17"
	architecture "x64"

	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}
	defines
		{
			"PENGINE_VULKAN",
		}
	
thirdparty = "%{wks.location}/project/thirdparty"
outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"
startproject "Editor"
include "project/src/pengine"
include "project/src/editor"
include "project/shader"
group "ThirdParty"
	include "project/thirdparty/GLFW"
	include "project/thirdparty/Glad"
	include "project/thirdparty/ImGui"
	include "project/thirdparty/spdlog"
	include "project/thirdparty/volk"
	include "project/thirdparty/VulkanSDK"
