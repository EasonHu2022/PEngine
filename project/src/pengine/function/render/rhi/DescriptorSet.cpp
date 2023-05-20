#include "DescriptorSet.h"
namespace pengine
{
	static bool updateValue = true;
	bool DescriptorSet::canUpdate()
	{
		return false;
	}
	void DescriptorSet::toggleUpdate(bool update)
	{
		updateValue = update;
	}
};