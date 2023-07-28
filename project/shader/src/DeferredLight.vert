#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec4 inColor;
layout(location = 2) in vec2 inTexCoord;
layout(location = 3) in vec3 inNormal;
layout(location = 4) in vec3 inTangent;

layout(location = 0) out vec2 fragTexCoord;

out gl_PerVertex
{
    vec4 gl_Position;
};

void main() 
{
   
    gl_Position = vec4(inPosition, inColor.x);//avoid optimize
    fragTexCoord = inTexCoord;
	//fragTexCoord.y = 1.0f - fragTexCoord.y;
}