#pragma once
#include "Component.h"
#include "glm/glm.hpp"
namespace pengine
{
	namespace component
	{
		class Mesh;

		constexpr glm::vec3 ZERO(0, 0, 0);
		constexpr glm::vec3 LEFT(-1.0f, 0.0f, 0.0f);
		constexpr glm::vec3 RIGHT(1.0f, 0.0f, 0.0f);
		constexpr glm::vec3 UP(0.0f, 1.0f, 0.0f);
		constexpr glm::vec3 DOWN(0.0f, -1.0f, 0.0f);
		constexpr glm::vec3 FORWARD(0.0f, 0.0f, 1.0f);
		constexpr glm::vec3 BACK(0.0f, 0.0f, -1.0f);
		constexpr glm::vec3 ONE(1.0f, 1.0f, 1.0f);
		class PENGINE_API Transform final: public Component
		{
		public:
			Transform();
			Transform(const glm::mat4& matrix);
			Transform(const glm::vec3& position);
			~Transform();
			auto setWorldMatrix(const glm::mat4& mat) -> void;
			auto setLocalTransform(const glm::mat4& localMat) -> void;
			auto setOffsetTransform(const glm::mat4& localMat) -> void;
			auto setLocalPosition(const glm::vec3& localPos) -> void;
			auto setLocalScale(const glm::vec3& localScale) -> void;
			auto setLocalOrientation(const glm::vec3& rotation) -> void;
			auto setLocalOrientation(const glm::quat& rotation) -> void;
			auto lookAt(const glm::vec3& target) -> void;
			inline auto& getWorldMatrix()
			{
				if (dirty)
					updateLocalMatrix();
				return worldMatrix;
			}

			inline const auto& getWorldMatrixInverse()
			{
				if (dirty)
					updateLocalMatrix();
				return worldMatrixInverse;
			}

			inline auto& getLocalMatrix()
			{
				if (dirty)
					updateLocalMatrix();
				return localMatrix;
			}
			inline auto getWorldPosition() const
			{
				return glm::vec3{ worldMatrix[3][0], worldMatrix[3][1], worldMatrix[3][2] };
			}
			inline auto getWorldOrientation() const
			{
				return glm::quat_cast(worldMatrix);
			};
			inline auto& getLocalPosition() const
			{
				return localPosition;
			}
			inline auto& getLocalScale() const
			{
				return localScale;
			}
			inline auto& getLocalOrientation() const
			{
				return localOrientation;
			}

			inline auto& getOffsetMatrix() const
			{
				return offsetMatrix;
			}

			inline auto hasUpdated() const
			{
				return hasUpdate;
			}
			inline auto setHasUpdated(bool set)
			{
				hasUpdate = set;
			}

			auto resetTransform() -> void;
			auto updateLocalMatrix() -> void;
			auto applyTransform() -> void;

			auto getUpDirection() const->glm::vec3;
			auto getRightDirection() const->glm::vec3;
			auto getForwardDirection() const->glm::vec3;

			static auto getScaleFromMatrix(const glm::mat4& mat) -> glm::vec3;
		protected:
			glm::mat4 localMatrix = glm::mat4(1);
			glm::mat4 worldMatrix = glm::mat4(1);
			glm::mat4 offsetMatrix = glm::mat4(1);
			glm::mat4 worldMatrixInverse = glm::mat4(1);

			glm::vec3 localPosition = {};
			glm::vec3 localScale = {};
			glm::vec3 localOrientation = {};

			glm::vec3 initLocalPosition = {};
			glm::vec3 initLocalScale = {};
			glm::vec3 initLocalOrientation = {};

			bool hasUpdate = false;
			bool dirty = false;
		};
	};
};