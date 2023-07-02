#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable
const float EPSILON = 0.00001;


layout(location = 0) in vec2 fragTexCoord;
layout(location = 0) out vec4 outColor;

layout(set = 0, binding = 0)  uniform sampler2D outputColor;

void main()
{
	outColor = texture(outputColor, fragTexCoord);
}


