#pragma once
#include <vector>
#include <memory>
#include "glm/glm.hpp"
namespace pengine
{
	class Texture;
	class TextureCube;
	class RenderPass;
	enum class TextureFormat : int32_t;
	enum class CubeFace : int32_t;
	
	
	struct FrameBufferInfo
	{
		uint32_t                              width;
		uint32_t                              height;
		uint32_t                              layer = 0;
		uint32_t                              msaaLevel = 0;
		bool                                  screenFBO = false;
		std::vector<std::shared_ptr<Texture>> attachments;
		std::shared_ptr<RenderPass>           renderPass;
	};

	class FrameBuffer
	{
	public:

		virtual ~FrameBuffer() = default;
		static std::shared_ptr<FrameBuffer> create(const FrameBufferInfo &info);
		virtual void validate() {};
		virtual void bind(uint32_t width, uint32_t height) const = 0;
		virtual void bind() const = 0;
		virtual void unbind() const = 0;
		virtual void clear() = 0;
		virtual void addTextureAttachment(TextureFormat format, const std::shared_ptr<Texture>& texture) = 0;
		virtual void addCubeTextureAttachment(TextureFormat format, CubeFace face, const std::shared_ptr<TextureCube> &texture) = 0;
		virtual void addShadowAttachment(const std::shared_ptr<Texture> &texture) = 0;
		virtual void addTextureLayer(int32_t index, const std::shared_ptr<Texture> &texture) = 0;
		virtual void generateFrameBuffer() = 0;
		virtual uint32_t getWidth() const = 0;
		virtual uint32_t getHeight() const = 0;
		virtual void setClearColor(const glm::vec4& color) = 0;
		virtual std::shared_ptr<Texture> getColorAttachment(int32_t id = 0)const = 0;
	private:

	};


};