
-- GLSLC helpers
dofile( "glslc.lua" )
project "shaders"
	kind "Utility"
	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")
	local shaders = { 
		"src/**.vert",
		"src/**.frag"
	}

	files( shaders )

	handle_glsl_files( "-g -O", "assets/shaders/spv", {})
