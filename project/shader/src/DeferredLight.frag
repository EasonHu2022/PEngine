#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable
#define PI 3.1415926535897932384626433832795
#define GAMMA 2.2
#define MAX_LIGHTS 32
#define NUM_VPL 256
#define MAX_SHADOWMAPS 4
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
layout(set = 0, binding = 4)  uniform sampler2DArray uShadowMap;
layout(set = 0, binding = 5)  uniform sampler2D uDepthSampler;
layout(set = 0, binding = 6) uniform UniformBufferLight
{
	Light lights[MAX_LIGHTS];
	mat4 viewMatrix;
	mat4 shadowTransform[MAX_SHADOWMAPS];
	mat4 biasMatrix;
	mat4 inverseVP;
	vec4 cameraPosition;
	vec4 splitDepths[MAX_SHADOWMAPS];
	float shadowMapSize;
	float maxShadowDistance;
	int lightCount;
	int mode;
	int enableIndirectLight;
	int shadowCount;
	int padding2;
} ubo;


vec4 debugColor;
float calculateShadow(vec3 wsPos);
float calculateCascadeIndex(vec3 wsPos);//caculate which layer is the fragment on
float textureProj(vec4 shadowCoord, vec2 offset, int cascadeIndex);
float PCSS(vec4 shadowCoords, int cascadeIndex);
float PCF(vec4 shadowCoords, int cascadeIndex);
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

float textureProj(vec4 shadowCoord, vec2 offset, int cascadeIndex)
{
	float shadow = 1.0;
	float ambient = 0.1;
	float initialBias = 0.005f;
	if ( shadowCoord.z > -1.0 && shadowCoord.z < 1.0 && shadowCoord.w > 0)
	{
		float dist = texture(uShadowMap, vec3(shadowCoord.xy + offset, cascadeIndex)).r;
		if (dist < (shadowCoord.z - initialBias))
		{
			shadow = ambient;//in;
		}
	}
	return shadow;
}

float calculateShadow(vec3 wsPos)
{
	float value = 0.0f;	
	int floorCascadeIndex = 0;
	int ceilCascadeIndex = 0;
	float fCascadeIndex = calculateCascadeIndex(wsPos);
	floorCascadeIndex = int(floor(fCascadeIndex));
	ceilCascadeIndex = int(ceil(fCascadeIndex));
	if( floorCascadeIndex == ceilCascadeIndex)
	{
		mat4 st =  (ubo.shadowTransform[floorCascadeIndex]);
		vec4 shadowCoord = st * vec4(wsPos,1.0f);
		shadowCoord = ubo.biasMatrix * shadowCoord;
		//shadowCoord.y = 1-shadowCoord.y;
		value = PCF(shadowCoord,floorCascadeIndex);
	}
	else
	{
		mat4 st =  (ubo.shadowTransform[floorCascadeIndex]);
		mat4 stNext =  (ubo.shadowTransform[ceilCascadeIndex]);
		vec4 shadowCoord = st * vec4(wsPos,1.0f);
		vec4 shadowCoordNext = stNext * vec4(wsPos,1.0f);
		//shadowCoord.xy = shadowCoord.yx;
		shadowCoord = ubo.biasMatrix * shadowCoord;
		shadowCoordNext = ubo.biasMatrix * shadowCoordNext;	
		//debugColor = vec4(shadowCoord.z,shadowCoord.z,shadowCoord.z,1.0);

		//shadowCoord.xy = (shadowwwCoord.xy + vec2(1.0f,1.0f)) / 2.0f;
		//because we set viewport height < 0, so here we need to flip y mannually
		//shadowCoord.y = 1-shadowCoord.y;
		//shadowCoordNext.y = 1-shadowCoordNext.y;

		//debugColor = texture(uShadowMap, vec3(shadowCoord.st, 1));
		//if enable PCF
		value = PCF(shadowCoord,floorCascadeIndex) * (ceilCascadeIndex - fCascadeIndex) + PCF(shadowCoordNext,ceilCascadeIndex) * (fCascadeIndex - floorCascadeIndex);
	}

	return value;
}

float calculateCascadeIndex(vec3 wsPos)
{
	float res = 0;
	int cascadeIndex = 0;
	vec4 viewPos = ubo.viewMatrix * vec4(wsPos, 1.0) ;
	
	for(int i = 0; i < ubo.shadowCount; ++i)
	{
		if(viewPos.z < ubo.splitDepths[i].y)
		{
			cascadeIndex = i + 1;
		}
	}
	if(viewPos.z < ubo.splitDepths[cascadeIndex].x)
	{
		res = cascadeIndex + (viewPos.z - ubo.splitDepths[cascadeIndex].x) / ( ubo.splitDepths[cascadeIndex].y -  ubo.splitDepths[cascadeIndex].x);
	}	
	else
	{
		res = cascadeIndex;
	}
	return res;
}

float PCSS(vec4 shadowCoords, int cascadeIndex)
{
	return 1.0f;
}
//https://blog.csdn.net/qq_35312463/article/details/117912599
float PCF(vec4 shadowCoords, int cascadeIndex)
{
	ivec2 texDim = textureSize(uShadowMap, 0).xy;
	float scale = 0.3f;
	float dx = scale * 1.0 / float(texDim.x);
	float dy = scale * 1.0 / float(texDim.y);

	float shadowFactor = 0.0;
	int count = 0;
	int range = 3;
	//Penumbra  sample/fake
	for (int x = -range; x <= range; x++) {
		for (int y = -range; y <= range; y++) {
			shadowFactor += textureProj(shadowCoords, vec2(dx*x, dy*y), cascadeIndex);
			count++;
		}
	}
	return shadowFactor / count;
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
	float fCascadeIndex = 0;
	int iCascadeIndex = 0;
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
		else if (light.type == 1.0)//spotlight
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
		else//0 directional light
		{
			//debugColor = texture(uShadowMap, vec3(shadowCoord.st, 1));
			//if enable PCF
			value = calculateShadow(wsPos);
			//if normal shadow
			//value = textureProj(shadowCoord,vec2(0.0),cascadeIndex);
		}
	
		
		vec3 Li = light.direction.xyz;
		vec3 Lradiance = lightColor;
		vec3 Lh = normalize(Li + material.view);
		
		// Calculate angles between surface normal and various light vectors.
		float cosLi = max(0.0, dot(material.normal, -1.0f *Li));
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

		//debug 
#if 0
{
		float cascadeIndex = calculateCascadeIndex(wsPos);
	if(cascadeIndex >= 0 && cascadeIndex <1)
	{
		if(floor(cascadeIndex) == ceil(cascadeIndex))
		{
			result.rgb *= vec3(1.0f, 0.25f, 0.25f);
		}
		else
		{
			result.rgb *= vec3(0.625f, 0.625f, 0.25f);
		}
		
	}
	if(cascadeIndex >= 1 && cascadeIndex < 2)
	{
		if(floor(cascadeIndex) == ceil(cascadeIndex))
		{
			result.rgb *= vec3(0.25f, 1.0f, 0.25f);
		}
		else
		{
			result.rgb *= vec3( 0.25f,0.625f, 0.625f);
		}
	}
	if(cascadeIndex >= 2 && cascadeIndex < 3)
	{
		if(floor(cascadeIndex) == ceil(cascadeIndex))
		{
			result.rgb *= vec3(0.25f, 0.25f, 1.0f);
		}
		else
		{
			result.rgb *= vec3(0.625f, 0.625f, 0.625f);
		}
	}
	if(int(cascadeIndex) == 3)
	{

		result.rgb *= vec3(1.0f, 1.0f, 0.25f);


	}
}	
#endif
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
	float _z = texture(uDepthSampler, fragTexCoord).r;
	vec2 uv = fragTexCoord.xy;
	vec4 Pos = (ubo.inverseVP * vec4((uv.xy * 2.0f - 1.0),_z,1.0f));
	Pos/=Pos.w;
	vec3 wsPos = Pos .xyz;

	vec4 albedo = texture(uColorSampler, uv);
	if (albedo.a < 0.1) {
		//discard;
	}
	vec4 fragPosXyzw = texture(uPositionSampler, uv);
	vec4 normalTex	 = texture(uNormalSampler, uv);
	vec4 pbr		 = texture(uPBRSampler,	uv);
	//wsPos = texture(uPositionSampler,	uv).xyz;
	Material material;
    material.albedo			= albedo;
    material.metallic		= vec3(pbr.x);
    material.roughness		= pbr.y;
    material.normal			= normalTex.xyz;
	material.ao				= pbr.z;
	material.ssao			= 1;

	
	material.view 			= normalize(ubo.cameraPosition.xyz -wsPos);
	material.normalDotView  = max(dot(material.normal, material.view), 0.0);


	//vec3 Lr =  reflect(-material.view,material.normal); 
	//2.0 * material.normalDotView * material.normal - material.view;
	// Fresnel reflectance, metals use albedo
	vec3 F0 = mix(Fdielectric, material.albedo.xyz, material.metallic.x);
	
	vec3 lightContribution = lighting(F0, wsPos, material,uv);

	vec3 finalColor = (lightContribution ) * material.ao * material.ssao;

	outColor = vec4(finalColor, 1.0) ;
	//outColor =  texture(uShadowMap, vec3(fragTexCoord, 1));
//	outColor.xyz = debugColor.xyz;
//	outColor.z = 0;
	//outColor.z = 0.;
	//outColor.z = 0;
	//outColor.xyz = wsPos / 1000.0f;
	//outColor.rgb = vec3(outColor.z); 
	//outColor.xyz = debugColor.xxx;
	//debug shadowmap
#if 0
	float depth = texture(uShadowMap, vec3(uv.xy,1)).r;
	outColor = vec4(depth,0.0,0.0,1.0);
#endif
}


