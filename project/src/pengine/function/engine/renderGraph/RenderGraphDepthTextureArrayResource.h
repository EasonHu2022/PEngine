#pragma once
#include "IRenderGraphResource.h"
#include "function/render/rhi/Texture.h"
#include <memory>
namespace pengine
{
	enum class TextureFormat;
	class DepthTextureArray;
	class CommandBuffer;
	class RenderGraphDepthTextureArrayResource : public IRenderGraphResource
	{
	public:
		RenderGraphDepthTextureArrayResource();
		~RenderGraphDepthTextureArrayResource();
		auto create(uint32_t width, uint32_t height, CommandBuffer* commandbuffer, const std::string& name, TextureFormat format, size_t layer) -> void override;
		auto getNativeResource() -> std::shared_ptr<Texture>
		{
			//PLOGV("get texture2d res use count : {0}",tex2D.use_count());
			return  std::static_pointer_cast<Texture>(depthArray);
		}
	private:
		std::shared_ptr<TextureDepthArray> depthArray;
	};

};