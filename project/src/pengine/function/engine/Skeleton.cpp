#include "Skeleton.h"
namespace pengine
{
	auto Skeleton::createBone(int32_t parentId) -> Bone&
	{
		auto& bone = bones.emplace_back();
		bone.id = bones.size() - 1;
		bone.parentIdx = parentId;
		return bone;
	}
	auto Skeleton::getBoneIndex(const std::string& name) -> int32_t
	{
		for (auto& bone : bones)
		{
			if (bone.name == name)
				return bone.id;
		}
		return -1;
	}
	auto Skeleton::buildRoot() -> void
	{
		roots.clear();
		for (auto& bone : bones)
		{
			if (bone.parentIdx == -1)
				roots.emplace_back(bone.id);
		}
	}
};