#pragma once
#include "IRenderGraphResource.h"
#include "function/render/rhi/Texture.h"
#include <memory>
namespace pengine
{
	enum class TextureFormat;
	class Texture2D;
	class CommandBuffer;
	class RenderGraphTexture2DResource: public IRenderGraphResource
	{
	public:
		RenderGraphTexture2DResource() ;
		~RenderGraphTexture2DResource();
		auto create(uint32_t width, uint32_t height, CommandBuffer* commandbuffer, const std::string& name, TextureFormat format, size_t layer = 1) -> void override;
		auto getNativeResource() -> std::shared_ptr<Texture>
		{
			//PLOGV("get texture2d res use count : {0}",tex2D.use_count());
			return  std::static_pointer_cast<Texture>(tex2D);
		}
	private:
		std::shared_ptr<Texture2D> tex2D;
	};

};