#pragma once
#include "Component.h"
#include "glm/glm.hpp"
namespace pengine
{
	struct CameraData
	{
		glm::mat4  proj;
		glm::mat4  view;
		glm::mat4  projView;
		glm::mat4  prevProjView;
		float      nearPlane;
		float      farPlane;
		float      fov;
	
	};
	class Camera : public Component
	{
	public:
		Camera();
		~Camera();

	private:
		CameraData cameraData;
	};
};
