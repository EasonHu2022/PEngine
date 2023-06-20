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

		struct LightData
		{
			glm::vec4 color;
			glm::vec4 position;
			glm::vec4 direction;
			float intensity;
			float radius;
			float type;
		};

		class Light : public Component
		{
		public:
			Light();
			~Light();

		private:
			LightData lightData;
		};
	};
};