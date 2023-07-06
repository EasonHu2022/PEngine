#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable
#define PI 3.1415926535897932384626433832795
#define GAMMA 2.2
#define MAX_LIGHTS 32
#define NUM_VPL 256
const float EPSILON = 0.00001;

// Constant normal incidence Fresnel factor for all dielectrics.
const vec3 Fdielectric = vec3(0.04);

struct Light
{
	vec4 color;
	vec4 position;
	vec4 direction;
	float intensity;
	float radius;
	float type;
	float angle;
};

struct Material
{
	vec4 albedo;
	vec3 metallic;
	float roughness;
	vec3 normal;
	float ao;
	float ssao;
	vec3 view;
	float normalDotView;
};


layout(location = 0) in vec2 fragTexCoord;
layout(location = 0) out vec4 outColor;

layout(set = 0, binding = 0)  uniform sampler2D uColorSampler;
layout(set = 0, binding = 1)  uniform sampler2D uPositionSampler;
layout(set = 0, binding = 2)  uniform sampler2D uNormalSampler;
layout(set = 0, binding = 3)  uniform sampler2D uPBRSampler;
layout(set = 0, binding = 4) uniform UniformBufferLight
{
	Light lights[MAX_LIGHTS];
	mat4 viewMatrix;
	vec4 cameraPosition;
	int lightCount;
	int mode;
	int enableIndirectLight;
	int padding2;
} ubo;


float rand(vec2 co)
{
    float a = 12.9898;
    float b = 78.233;
    float c = 43758.5453;
    float dt= dot(co.xy ,vec2(a,b));
    float sn= mod(dt,3.14);
    return fract(sin(sn) * c);
}

float random(vec4 seed4)
{
	float dotProduct = dot(seed4, vec4(12.9898,78.233,45.164,94.673));
    return fract(sin(dotProduct) * 43758.5453);
}



// GGX/Towbridge-Reitz normal distribution function.
// Uses Disney's reparametrization of alpha = roughness^2
float ndfGGX(float cosLh, float roughness)
{
	float alpha = roughness * roughness;
	float alphaSq = alpha * alpha;
	
	float denom = (cosLh * cosLh) * (alphaSq - 1.0) + 1.0;
	return alphaSq / (PI * denom * denom);
}

// Single term for separable Schlick-GGX below.
float gaSchlickG1(float cosTheta, float k)
{
	return cosTheta / (cosTheta * (1.0 - k) + k);
}

// Schlick-GGX approximation of geometric attenuation function using Smith's method.
float gaSchlickGGX(float cosLi, float NdotV, float roughness)
{
	float r = roughness + 1.0;
	float k = (r * r) / 8.0; // Epic suggests using this roughness remapping for analytic lights.
	return gaSchlickG1(cosLi, k) * gaSchlickG1(NdotV, k);
}

// Shlick's approximation of the Fresnel factor.
vec3 fresnelSchlick(vec3 F0, float cosTheta)
{
	return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

vec3 fresnelSchlickRoughness(vec3 F0, float cosTheta, float roughness)
{
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

vec3 lighting(vec3 F0, vec3 wsPos, Material material,vec2 fragTexCoord)
{
	vec3 result = vec3(0.0);
	
	for(int i = 0; i < ubo.lightCount; i++)
	{
		Light light = ubo.lights[i];
		float value = 1.0;
		vec3 lightColor = light.color.xyz * light.intensity;
		vec3 indirect = vec3(0,0,0);
		if(light.type == 2.0)//pointlight
		{
		    // Vector to light
			vec3 L = light.position.xyz - wsPos;
			// Distance from light to fragment position
			float dist = length(L);
			
			// Light to fragment
			L = normalize(L);
			
			// Attenuation
			float radius = light.radius;  
			float attenuationDistance = radius * 2.0;  

			float constant = 1.0;
			float linear = 2.0 / attenuationDistance;  
			float quadratic = 1.0 / (attenuationDistance * attenuationDistance);
			float atten = 1.0f/(constant +  dist * linear + pow(dist, 2.0) * quadratic);

			value = atten;
			
			light.direction = vec4(L,1.0);
		}
		if (light.type == 1.0)//spotlight
		{
			vec3 L = light.position.xyz - wsPos;
			float cutoffAngle   = 1.0f - light.angle;      
			float dist          = length(L);
			L = normalize(L);
			float theta         = dot(L.xyz, light.direction.xyz);
			float epsilon       = cutoffAngle - cutoffAngle * 0.9f;
			float attenuation 	= ((theta - cutoffAngle) / epsilon); // atteunate when approaching the outer cone
			attenuation         *= light.radius / (pow(dist, 2.0) + 1.0);//saturate(1.0f - dist / light.range);
			float intensity 	= attenuation * attenuation;
			// Erase light if there is no need to compute it
			intensity *= step(theta, cutoffAngle);
			value = clamp(attenuation, 0.0, 1.0);
		}
	
		
		vec3 Li = light.direction.xyz;
		vec3 Lradiance = lightColor;
		vec3 Lh = normalize(Li + material.view);
		
		// Calculate angles between surface normal and various light vectors.
		float cosLi = max(0.0, dot(material.normal, Li));
		float cosLh = max(0.0, dot(material.normal, Lh));
		
		//vec3 F = fresnelSchlick(F0, max(0.0, dot(Lh, material.view)));
		vec3 F = fresnelSchlickRoughness(F0, max(0.0, dot(Lh,  material.view)), material.roughness);
		
		float D = ndfGGX(cosLh, material.roughness);
		float G = gaSchlickGGX(cosLi, material.normalDotView, material.roughness);
		
		vec3 kd = (1.0 - F) * (1.0 - material.metallic.x);
		vec3 diffuseBRDF = kd * material.albedo.xyz / PI;
		
		// Cook-Torrance
		vec3 specularBRDF = (F * D * G) / max(EPSILON, 4.0 * cosLi * material.normalDotView);
		
		vec3 directShading = (diffuseBRDF + specularBRDF) * Lradiance * cosLi * value;
		vec3 indirectShading = (diffuseBRDF + specularBRDF) * indirect * cosLi;
		vec3 ambient = material.albedo.xyz * vec3(0.1f,0.1f,0.1f);
		result += directShading + indirectShading + ambient;
		//result += indirect;
	}
	//ambient
	return result;
}

vec3 gammaCorrectTextureRGB(vec3 texCol)
{
	return vec3(pow(texCol.rgb, vec3(GAMMA)));
}

float attentuate( vec3 lightData, float dist )
{
	float att =  1.0 / ( lightData.x + lightData.y*dist + lightData.z*dist*dist );
	float damping = 1.0;
	return max(att * damping, 0.0);
}

void main()
{
	vec4 albedo = texture(uColorSampler, fragTexCoord);
	if (albedo.a < 0.1) {
		discard;
	}
	vec4 fragPosXyzw = texture(uPositionSampler, fragTexCoord);
	vec4 normalTex	 = texture(uNormalSampler, fragTexCoord);
	vec4 pbr		 = texture(uPBRSampler,	fragTexCoord);
	
	Material material;
    material.albedo			= albedo;
    material.metallic		= vec3(pbr.x);
    material.roughness		= pbr.y;
    material.normal			= normalTex.xyz;
	material.ao				= pbr.z;
	material.ssao			= 1;

	vec3 wsPos = fragPosXyzw.xyz;
	material.view 			= normalize(ubo.cameraPosition.xyz -wsPos);
	material.normalDotView  = max(dot(material.normal, material.view), 0.0);

	
	vec4 viewPos = ubo.viewMatrix * vec4(wsPos, 1.0);
	
	float distance = length(viewPos);	
	vec3 Lr =  reflect(-material.view,material.normal); 
	//2.0 * material.normalDotView * material.normal - material.view;
	// Fresnel reflectance, metals use albedo
	vec3 F0 = mix(Fdielectric, material.albedo.xyz, material.metallic.x);
	
	vec3 lightContribution = lighting(F0, wsPos, material,fragTexCoord);

	vec3 finalColor = (lightContribution ) * material.ao * material.ssao;

	outColor = vec4(finalColor, 1.0) ;

}


