#pragma once
#include "ITask.h"
namespace pengine
{
	class RenderGBufferTask : public ITask
	{
	public:
		RenderGBufferTask(uint32_t	_uid);
		~RenderGBufferTask();
		auto init() -> void;
		auto execute() -> void;
		auto setup() -> void;
	private:

	};

};