#pragma once
#include "VKHelper.h"
#include "core/core.h"
namespace pengine
{
	class VulkanCommandPool
	{
	public:
		VulkanCommandPool(int32_t queueIndex, VkCommandPoolCreateFlags flags);
		~VulkanCommandPool();
		NO_COPYABLE(VulkanCommandPool);
		void reset();
		inline operator auto() const 
		{                            
			return commandPool;
		}
		inline auto getHandle() const
		{
			return commandPool;
		}
	private:
		VkCommandPool commandPool;
	};
}