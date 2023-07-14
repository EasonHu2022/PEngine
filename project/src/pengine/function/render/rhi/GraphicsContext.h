#pragma once
#include <memory>
#include <unordered_map>
#include "core/core.h"
namespace pengine
{
	enum class GraphicsAPI : uint32_t
	{
		OPENGL = 0,
		VULKAN,
		DIRECT3D,
		METAL,
		NONE,
	};

	template <typename T>
	struct CacheAsset
	{
		CacheAsset(std::shared_ptr<T> asset, uint64_t lastTimestamp) :
			asset(asset), lastTimestamp(lastTimestamp) {};
		std::shared_ptr<T> asset;
		uint64_t           lastTimestamp;
	};

	class CommandBuffer;
	class SwapChain;
	class Pipeline;
	class FrameBuffer;
	class Shader;

	class GraphicsContext
	{
	public:
		virtual ~GraphicsContext() = default;
		inline static auto getGraphicsAPI()
		{
#if defined(PENGINE_OPENGL)
			return GraphicsAPI::OPENGL;
#elif defined(PENGINE_VULKAN)
			return GraphicsAPI::VULKAN;
#endif        
			return GraphicsAPI::NONE;
		}
		virtual void init() = 0;
		virtual void release() = 0;
		virtual void present() = 0;
		virtual void waitIdle() const = 0;
		virtual void onImGui() = 0;
		virtual size_t getMinUniformBufferOffsetAlignment() const = 0;
		virtual float getGPUMemoryUsed() = 0;
		virtual float getTotalGPUMemory() = 0;
		virtual void forceFlushDeletionQueue() = 0;
		virtual void delayFlushDeletionQueue() = 0;
		static std::shared_ptr<GraphicsContext> create();
		inline auto getSwapChain() -> std::shared_ptr<SwapChain>
		{
			PENGINE_ASSERT(swapChain != nullptr, "SwapChain must be initialized");
			return swapChain;
		}

		inline auto& getPipelineCache()
		{
			return pipelineCache;
		}

		inline auto& getFrameBufferCache()
		{
			return frameBufferCache;
		}

		auto clearUnused() -> void;
		//forcely clear all caches at once
		auto forceClearImmediately() -> void;
	protected:
		std::shared_ptr<SwapChain>                               swapChain;
		std::unordered_map<std::size_t, CacheAsset<Pipeline>>    pipelineCache;
		std::unordered_map<std::size_t, CacheAsset<FrameBuffer>> frameBufferCache;
	};
}