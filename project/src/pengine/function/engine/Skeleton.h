#pragma once
#include <vector>
#include <string>
#include "glm/glm.hpp"
namespace pengine
{
	struct Bone
	{
		int32_t id;
		int32_t parentIdx;
		std::string name;
		std::vector<int32_t> children;

		glm::vec3 position;
		glm::vec3 rotation;

		glm::vec3 restPosition;
		glm::vec3 restRotation;

		glm::mat4 offsetMatrix;
		glm::mat4 worldTransform;
	};

	class Skeleton
	{
	public:
		auto createBone(int32_t parentId = -1) -> Bone&;
		auto getBoneIndex(const std::string& name) -> int32_t;
		auto buildRoot() -> void;
		inline auto getRoots() const { return roots; }
		inline auto& getBones() const { return bones; }
		inline auto& getBones() { return bones; }
		inline auto isValidBoneIndex(int32_t index) const { return index < bones.size() && index >= 0; }
	private:
		std::vector<Bone> bones;
		std::vector<int32_t> roots;
	};

};