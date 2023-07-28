#include "Transform.h"
#include <glm/gtx/matrix_decompose.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
namespace pengine
{
	namespace component
	{
		Transform::Transform()
		{
			localPosition = { 0.0f, 0.0f, 0.0f };
			localOrientation = {};
			localScale = { 1.0f, 1.0f, 1.0f };
			localMatrix = glm::mat4(1.f);
			worldMatrix = glm::mat4(1.f);

			initLocalPosition = localPosition;
			initLocalScale = localScale;
			initLocalOrientation = localOrientation;
		}
		Transform::Transform(const glm::mat4& matrix)
		{
			glm::vec3 skew;
			glm::quat rotation;
			glm::vec4 perspective;
			glm::decompose(matrix, localScale, rotation, localPosition, skew, perspective);
			localOrientation = glm::eulerAngles(rotation);
			localMatrix = matrix;
			worldMatrix = matrix;
			worldMatrixInverse = glm::inverse(worldMatrix);

			initLocalPosition = localPosition;
			initLocalScale = localScale;
			initLocalOrientation = localOrientation;
		}
		Transform::Transform(const glm::vec3& position)
		{
			localPosition = position;
			localOrientation = {};
			localScale = { 1.0f, 1.0f, 1.0f };
			localMatrix = glm::mat4(1.f);
			worldMatrix = glm::mat4(1.f);
			setLocalPosition(position);

			initLocalPosition = localPosition;
			initLocalScale = localScale;
			initLocalOrientation = localOrientation;
		}

		Transform::~Transform() = default;

		auto Transform::setWorldMatrix(const glm::mat4& mat) -> void
		{
			if (dirty)
				updateLocalMatrix();
			worldMatrix = mat * localMatrix;
			worldMatrixInverse = glm::inverse(worldMatrix);
		}
		auto Transform::setLocalTransform(const glm::mat4& localMat) -> void
		{
			localMatrix = localMat;
			hasUpdate = true;
			applyTransform();        
		}
		auto Transform::setOffsetTransform(const glm::mat4& localMat) -> void
		{
			offsetMatrix = localMat;
		}
		auto Transform::setLocalPosition(const glm::vec3& localPos) -> void
		{
			dirty = true;
			localPosition = localPos;
		}
		auto Transform::setLocalScale(const glm::vec3& _localScale) -> void
		{
			dirty = true;
			localScale = _localScale;
		}
		auto Transform::setLocalOrientation(const glm::vec3& rotation) -> void
		{
			dirty = true;
			localOrientation = rotation;
		}
		auto Transform::setLocalOrientation(const glm::quat& rotation) -> void
		{
			dirty = true;
			localOrientation = glm::eulerAngles(rotation);
		}
		auto Transform::lookAt(const glm::vec3& target) -> void
		{
			localMatrix = glm::lookAt(localPosition, target, getUpDirection());
			applyTransform();
			dirty = true;
		}
		auto Transform::resetTransform() -> void
		{
			dirty = true;
			localPosition = initLocalPosition;
			localScale = initLocalScale;
			localOrientation = initLocalOrientation;
		}
		auto Transform::updateLocalMatrix() -> void
		{
			glm::mat4 t = glm::translate(glm::mat4(1), localPosition);
			glm::mat4 r = glm::toMat4(glm::quat(localOrientation));
			glm::mat4 s = glm::scale(glm::mat4(1), localScale);
			//localMatrix = glm::translate(glm::mat4(1), localPosition);
			//localMatrix *= glm::toMat4(glm::quat(localOrientation));
			//localMatrix = glm::scale(localMatrix, localScale);
			
			localMatrix = t * r * s;
			
			dirty = false;
			hasUpdate = true;
		}
		auto Transform::applyTransform() -> void
		{
			glm::vec3 skew;
			glm::vec4 perspective;
			glm::quat rotation;
			glm::decompose(localMatrix, localScale, rotation, localPosition, skew, perspective);
			localOrientation = glm::eulerAngles(rotation);
		}
		auto Transform::getUpDirection() const -> glm::vec3
		{
			return getWorldOrientation() * UP;
		}
		auto Transform::getRightDirection() const -> glm::vec3
		{
			return getWorldOrientation() * RIGHT;
		}
		auto Transform::getForwardDirection() const -> glm::vec3
		{
			return getWorldOrientation() * FORWARD;
		}
		auto Transform::getScaleFromMatrix(const glm::mat4& mat) -> glm::vec3
		{
			glm::vec3 skew;
			glm::vec3 localScale;
			glm::quat localOrientation;
			glm::vec3 localPosition;
			glm::vec4 perspective;
			glm::decompose(mat, localScale, localOrientation, localPosition, skew, perspective);
			return localScale;
		}
	};
};