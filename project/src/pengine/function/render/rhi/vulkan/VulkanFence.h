#pragma once
#include "VKHelper.h"
#include "core/core.h"
namespace pengine
{
	class VulkanFence
	{
	public:
		VulkanFence(bool createSignaled = false);
		~VulkanFence();
		NO_COPYABLE(VulkanFence);
		void reset();
		bool wait();
		void waitAndReset();
		bool checkState() ;
		bool isSignaled() ;
		inline operator auto() const 
		{                            
			return fence;                
		}
		inline auto setSignaled(bool signaled)
		{
			this->signaled = signaled;
		}
	private:
		VkFence fence;
		bool    signaled;
	};

};