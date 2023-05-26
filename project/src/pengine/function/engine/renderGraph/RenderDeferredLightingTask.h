#pragma once
#include "ITask.h"
namespace pengine
{
	class RenderDeferredLightingTask : public ITask
	{
	public:
		RenderDeferredLightingTask(uint32_t uid);
		~RenderDeferredLightingTask();
		auto init() -> void;
		auto execute() -> void;
		auto setup() -> void;
	private:
	};
};