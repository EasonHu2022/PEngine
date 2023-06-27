#include "BoundingBox.h"

namespace pengine 
{
	auto BoundingBox::transform(const glm::mat4& transform) -> BoundingBox
	{
		auto min = transform * glm::vec4(this->min, 1.f);
		auto max = transform * glm::vec4(this->max, 1.f);
		return {min,max};
	}
};

