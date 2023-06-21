#include "core/core.h"
#include "PassGroup.h"
#include "function/render/rhi/CommandBuffer.h"
#include "RenderGraph.h"
namespace pengine
{
	auto PassGroup::execute(CommandBuffer* cmdBuffer) -> void
	{
	}
	auto PassGroup::compile() -> void
	{
		//record group dp map
		for (auto id : passes)
		{
			auto& pass = m_renderGraph->getPassByID(id);
		}
	}
	auto PassGroup::addPass(uint32_t id) -> void
	{
		if (std::find(passes.begin(), passes.end(), id) != passes.end())
		{
			PLOGE(" try to add an exists pass into a group, pass id : {0}, ",id);
			return;
		}
		passes.push_back(id);
	}
};