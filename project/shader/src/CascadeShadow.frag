#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable
layout(location = 0) in vec2 fragTexCoord;
layout(location = 1) in vec4 worldPosition;
layout(location = 0) out vec4 debugColor;

void main()
{
	debugColor = worldPosition;
}


