#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable
layout(set = 0,binding = 0) uniform UniformBufferObject 
{    
	mat4 projView;
    mat4 view;
	mat4 projViewOld;
} ubo;

layout(push_constant) uniform PushConsts
{
	mat4 transform;
} pushConsts;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec4 inColor;
layout(location = 2) in vec2 inTexCoord;
layout(location = 3) in vec3 inNormal;
layout(location = 4) in vec3 inTangent;


layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragTexCoord;
layout(location = 2) out vec4 fragPosition;
layout(location = 3) out vec3 fragNormal;
layout(location = 4) out vec3 fragTangent;
layout(location = 5) out vec4 fragProjPosition;
layout(location = 6) out vec4 fragOldProjPosition;
layout(location = 7) out vec4 fragViewPosition;



out gl_PerVertex
{
    vec4 gl_Position;
};

void main() 
{
	fragPosition = pushConsts.transform * vec4(inPosition, 1.0);
    vec4 pos =  ubo.projView * fragPosition;
    gl_Position = pos;
    
    fragColor = inColor.xyz;
	fragTexCoord = inTexCoord;
    fragNormal =  transpose(inverse(mat3(pushConsts.transform))) * normalize(inNormal);
    fragNormal = inNormal;
    fragTangent = inTangent;

    fragProjPosition = pos;
    fragOldProjPosition = ubo.projViewOld * pushConsts.transform * vec4(inPosition, 1.0);;
    fragViewPosition = ubo.view * fragPosition;
}