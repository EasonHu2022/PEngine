#pragma once
#include "core/core.h"
#include "IRenderGraphResource.h"
#include <vector>
#include <memory>
namespace pengine
{
	class ITask
	{
	public:
		virtual auto execute() -> void;
	private:

	};


};