#include "Light.h"
namespace pengine
{
	namespace component
	{
		
		Light::Light(const glm::vec3& direction, const glm::vec4& color, float intensity, const LightType& type, const glm::vec3& position, float radius, float angle)
		{
			lightData.direction = { direction,1.f };
			lightData.color = color;
			lightData.position = { position,1.f };
			lightData.intensity = intensity;
			lightData.radius = radius;
			lightData.type = static_cast<float>(type);
			lightData.angle = angle;
		}
		
	};
}