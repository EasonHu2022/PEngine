#pragma once
#include <vector>
#include <unordered_map>
namespace pengine
{
	class CommandBuffer;
	class RenderGraph;
	class PassGroup
	{
	public:
		PassGroup(RenderGraph* renderGraph):m_renderGraph(renderGraph){};
		~PassGroup() {};
		auto compile() -> void;
		auto execute(CommandBuffer* cmdBuffer) -> void;
		auto addPass(uint32_t id) -> void;
		inline auto getPass(size_t index) -> uint32_t { return passes[index]; };
		inline auto getPassesCount() { return passes.size(); }

	private:
		std::vector<uint32_t> passes;
		RenderGraph* m_renderGraph;
	};
};