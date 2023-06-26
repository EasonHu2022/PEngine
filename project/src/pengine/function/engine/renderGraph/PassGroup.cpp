#include "core/core.h"
#include "PassGroup.h"
#include "function/render/rhi/CommandBuffer.h"
#include "RenderGraph.h"
namespace pengine
{
	auto PassGroup::execute(CommandBuffer* cmdBuffer) -> void
	{
		for (auto pass : passes)
		{
			m_renderGraph->getPassByID(pass)->execute(cmdBuffer);
		}
	}
	auto PassGroup::compile() -> void
	{
		//TODO--
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