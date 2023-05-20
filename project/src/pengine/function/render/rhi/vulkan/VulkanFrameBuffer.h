#pragma once
#include "function/render/rhi/FrameBuffer.h"
#include "core/core.h"
#include "VKHelper.h"
namespace pengine
{
	class VulkanFrameBuffer : public FrameBuffer
	{
	public:
		VulkanFrameBuffer(const FrameBufferInfo &info);
		~VulkanFrameBuffer();
		NO_COPYABLE(VulkanFrameBuffer);
		inline auto bind(uint32_t width, uint32_t height) const -> void {};
		inline auto bind() const -> void override {};
		inline auto unbind() const -> void override {};
		inline auto clear() -> void override {};
		inline auto addTextureAttachment(TextureFormat format, const std::shared_ptr<Texture>& texture) -> void override {};
		inline auto addCubeTextureAttachment(TextureFormat format, CubeFace face, const std::shared_ptr<TextureCube>& texture) -> void override {};
		inline auto addShadowAttachment(const std::shared_ptr<Texture>& texture) -> void override {};
		inline auto addTextureLayer(int32_t index, const std::shared_ptr<Texture>& texture) -> void override {};
		inline auto generateFrameBuffer() -> void override {};
		inline auto setClearColor(const glm::vec4& color) -> void override {};
		inline auto getColorAttachment(int32_t id = 0) const -> std::shared_ptr<Texture> override
		{
			return nullptr;
		};

		inline auto& getFrameBufferInfo() const
		{
			return info;
		}
		inline auto getWidth() const -> uint32_t override
		{
			return info.width;
		};
		inline auto getHeight() const -> uint32_t override
		{
			return info.height;
		};
		inline operator auto() const
		{
			return buffer;
		}
	private:
		FrameBufferInfo info;
		VkFramebuffer  buffer = VK_NULL_HANDLE;
		
		

	};

};