#pragma once
#include "ITask.h"
#include "IRenderGraphResource.h"
namespace pengine
{
	class RenderGraph
	{
	public:
		RenderGraph();
		~RenderGraph();
		auto execute() -> void;
	private:
		std::vector<ITask*> tasks;
		std::vector<IRenderGraphResource*> resources;
	};

};