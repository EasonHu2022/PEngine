#pragma once
#include <memory>
namespace pengine
{
	class RenderPass;
	class FrameBuffer;
	class Pipeline;
	class CommandBuffer
	{
	public:
		virtual ~CommandBuffer() = default;
		static std::shared_ptr<CommandBuffer> create();
		virtual bool init(bool primary) = 0;
		virtual void unload() = 0;
		virtual void beginRecording() = 0;
		virtual void beginRecordingSecondary(RenderPass* renderPass, FrameBuffer* framebuffer) = 0;
		virtual void endRecording() = 0;
		virtual void executeSecondary(CommandBuffer* primaryCmdBuffer) = 0;
		virtual void updateViewport(uint32_t width, uint32_t height) = 0;
		virtual void bindPipeline(Pipeline* pipeline) = 0;
		virtual void unbindPipeline() = 0;
		virtual bool isRecording() const 
		{
			return true;
		};
		virtual bool flush() 
		{
			return true;
		}

	};


};