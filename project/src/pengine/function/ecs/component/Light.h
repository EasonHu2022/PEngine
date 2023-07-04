#pragma once
#include "glm/glm.hpp"
#include "Component.h"
namespace pengine
{
	namespace component
	{
		enum class LightType
		{
			DirectionalLight = 0,
			SpotLight = 1,
			PointLight = 2,
		};
		//gpu layout data
		struct LightData
		{
			glm::vec4 color;
			glm::vec4 position;
			glm::vec4 direction;
			float intensity;
			float radius;
			float type;
			float angle;
		};

		class Light : public Component
		{
		public:
			Light(const glm::vec3& direction = glm::vec3(0.0f), const glm::vec4& color = glm::vec4(1.0f), float intensity = 3.0f, const LightType& type = LightType::PointLight, const glm::vec3& position = glm::vec3(), float radius = 5000.0f, float angle = 0.0f);
			LightData lightData;
		};
	};
};