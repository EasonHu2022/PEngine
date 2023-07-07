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
			"GLM_FORCE_DEPTH_ZERO_TO_ONE",
		}
	
thirdparty = "%{wks.location}/project/thirdparty"
assetsdir = "%{wks.location}/assets"
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
	include "project/thirdparty/openFBX"
	include "project/thirdparty/SPIRV-Cross"
	include "project/thirdparty/tracy"
