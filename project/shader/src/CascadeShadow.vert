#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable
layout(push_constant) uniform PushConsts
{
	mat4 transform;
	uint cascadeIndex;
} pushConsts;
layout(set = 0,binding = 0) uniform UniformBufferObject
{
    mat4 projView[4];
} ubo;
out gl_PerVertex
{
    vec4 gl_Position;
};
layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec4 inColor;
layout(location = 2) in vec2 inTexCoord;
layout(location = 3) in vec3 inNormal;
layout(location = 4) in vec3 inTangent;


void main() 
{
    gl_Position = ubo.projView[pushConsts.cascadeIndex] * pushConsts.transform *  vec4(inPosition, inColor.x);// avoid optimize 
}