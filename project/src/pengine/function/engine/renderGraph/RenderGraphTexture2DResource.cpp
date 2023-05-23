#include "RenderGraphTexture2DResource.h"
#include "function/render/rhi/CommandBuffer.h"
#include "function/render/rhi/Texture.h"
namespace pengine
{
	auto RenderGraphTexture2DResource::create(uint32_t width, uint32_t height, CommandBuffer* commandbuffer, const std::string& name) -> void
	{
		tex2D = Texture2D::create();
		tex2D->setName(name);
		tex2D->buildTexture(TextureFormat::RGBA16, width, height, false, false, false);
	}
}