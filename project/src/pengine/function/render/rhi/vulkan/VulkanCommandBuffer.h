#pragma once
#include "VKHelper.h"
#include "core/core.h"
#include "function/render/rhi/CommandBuffer.h"
#include "VulkanFence.h"
namespace pengine
{
	enum class CommandBufferState : uint8_t
	{
		Idle,
		Recording,
		Ended,
		Submitted
	};

	class VulkanCommandBuffer : public CommandBuffer
	{
	public:
		VulkanCommandBuffer();
		VulkanCommandBuffer(VkCommandBuffer commandBuffer);
		~VulkanCommandBuffer();
		NO_COPYABLE(VulkanCommandBuffer);
		bool init(bool primary) override;
		bool init(bool primary, VkCommandPool commandPool) ;
		void unload()  override;
		void beginRecording()  override;
		void beginRecordingSecondary(RenderPass* renderPass, FrameBuffer* framebuffer)  override;
		void endRecording()  override;
		void executeSecondary(CommandBuffer* primaryCmdBuffer)  override;
		void updateViewport(uint32_t width, uint32_t height)  override;
		void bindPipeline(Pipeline* pipeline)  override;
		void unbindPipeline()  override;
		bool flush()  override;

		inline bool isRecording() const  override
		{
			return state == CommandBufferState::Recording;
		}

		inline auto getState() const
		{
			return state;
		}

		inline auto getSemaphore() const
		{
			return semaphore;
		}

		void wait() ;
		void reset() ;

		void executeInternal(VkPipelineStageFlags flags, VkSemaphore signalSemaphore, bool waitFence)  ;

		inline auto getCommandBuffer() const
		{
			return commandBuffer;
		}

	private:
		VkCommandBuffer              commandBuffer;
		VkCommandPool                commandPool = VK_NULL_HANDLE;
		bool                         primary;
		CommandBufferState           state = CommandBufferState::Idle;
		std::shared_ptr<VulkanFence> fence;
		VkSemaphore                  semaphore = VK_NULL_HANDLE;

		Pipeline* boundPipeline = nullptr;
		RenderPass* boundRenderPass = nullptr;
	};

};