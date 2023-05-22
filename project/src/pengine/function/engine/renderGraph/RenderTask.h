#pragma once
#include "ITask.h"
namespace pengine
{
	class RenderTask : public ITask
	{
	public:
		RenderTask();
		~RenderTask();
		auto execute() -> void override;
	private:

	};

};