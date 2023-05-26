#pragma once
#include "IRenderGraphResource.h"
#include <memory>
namespace pengine
{
	enum class TextureFormat;
	class Texture2D;
	class CommandBuffer;
	class RenderGraphTexture2DResource: public IRenderGraphResource
	{
	public:
		auto create(uint32_t width, uint32_t height, CommandBuffer* commandbuffer, const std::string& name, TextureFormat format) -> void override;
	private:
		std::shared_ptr<Texture2D> tex2D;
	};

};