#pragma once
#include "Component.h"
#include "glm/glm.hpp"
namespace pengine
{
	namespace component {
	
		class Camera : public Component
		{
		public:
			Camera() = default;
			Camera(float fov, float near_, float far_, float aspect);
			Camera(float aspectRatio, float scale);
			Camera(float pitch, float yaw, const glm::vec3& position, float fov, float near_, float far_, float aspect);
			~Camera() = default;
			auto getProjection() -> const glm::mat4&;
			auto getPrevProjection() -> const glm::mat4&;
			auto getPrevProjectionView() -> const glm::mat4& { return prevProjMatrix; };
		public:
			inline auto setOrthographic(bool ortho)
			{
				orthographic = ortho;
				projectionDirty = true;
			}

			inline auto isOrthographic() const
			{
				return orthographic;
			}

			inline auto getAspectRatio() const
			{
				return aspectRatio;
			}

			inline auto setAspectRatio(float y)
			{
				aspectRatio = y;
				projectionDirty = true;
			};

			inline auto& getFar() const
			{
				return far_;
			}
			inline auto& getNear() const
			{
				return near_;
			}
			inline auto setFar(float pFar)
			{
				far_ = pFar;
				projectionDirty = true;
			}
			inline auto setNear(float pNear)
			{
				near_ = pNear;
				projectionDirty = true;
			}
			inline auto getFov() const
			{
				return fov;
			}
			inline auto getScale() const
			{
				return scale;
			}
			inline auto setScale(float scale)
			{
				this->scale = scale;
				projectionDirty = true;
			}
			inline auto setFov(float fov)
			{
				this->fov = fov;
				projectionDirty = true;
			}
		protected:
			auto updateProjectionMatrix() -> void;

			float aspectRatio = 0.0f;
			float scale = 100.0f;
			float zoom = 1.0f;

			glm::mat4 projMatrix = glm::mat4(1.f);
			glm::mat4 prevProjMatrix = glm::mat4(1.f);
			bool      projectionDirty = false;

			float mouseSensitivity = 0.1f;
			bool  orthographic = false;

			float fov = 0.0f;
			float near_ = 0.0f;
			float far_ = 0.0f;
		};
	};
	
};
