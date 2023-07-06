#include "Camera.h"
#include "glm/glm.hpp"
#include <glm/gtc/matrix_transform.hpp>
namespace pengine
{
	namespace component
	{

		Camera::Camera(float fov, float near_, float far_, float aspect) : 
			aspectRatio(aspect), projectionDirty(true), fov(fov), near_(near_), far_(far_), orthographic(false)
		{
		}

		Camera::Camera(float aspectRatio, float scale)
			: aspectRatio(aspectRatio), scale(scale), projectionDirty(true), fov(0), near_(1.0), far_(3200000.0f), orthographic(true)
		{
		}

		Camera::Camera(float pitch, float yaw, const glm::vec3& position, float fov, float near_, float far_, float aspect)
			:aspectRatio(aspect), projectionDirty(true), fov(fov), near_(near_), far_(far_), orthographic(false)
		{
		}

		auto Camera::getProjection() -> const glm::mat4&
		{
			if (projectionDirty)
			{
				prevProjMatrix = projMatrix;
				updateProjectionMatrix();
				projectionDirty = false;
			}
			return projMatrix;
		}

		auto Camera::getPrevProjection() -> const glm::mat4&
		{
			if (projectionDirty)
			{
				prevProjMatrix = projMatrix;
				updateProjectionMatrix();
				projectionDirty = false;
			}
			return prevProjMatrix;
		}

		auto Camera::getFrustum(const glm::mat4& viewMatrix) -> const Frustum&
		{
			if (projectionDirty)
			{
				updateProjectionMatrix();
				projectionDirty = false;
			}
			frustum.from(projMatrix * viewMatrix);
			return frustum;
		}

		auto Camera::updateProjectionMatrix() -> void
		{


			if (orthographic)
				projMatrix = glm::ortho(aspectRatio * -scale, aspectRatio * scale, -scale, scale, near_, far_);
			else
				projMatrix = glm::perspective(glm::radians(fov), aspectRatio, near_, far_);


			
		}

	};
};
