#include "RenderGraphTexture2DResource.h"
#include "function/render/rhi/CommandBuffer.h"
#include "function/render/rhi/Texture.h"
namespace pengine
{
	RenderGraphTexture2DResource::RenderGraphTexture2DResource()
	{
	}
	RenderGraphTexture2DResource::~RenderGraphTexture2DResource()
	{
		//PLOGV("texture use count in render graph resource : {0}", tex2D.use_count());
		tex2D.reset();
	}
	auto RenderGraphTexture2DResource::create(uint32_t width, uint32_t height,
		CommandBuffer* commandbuffer, const std::string& name, TextureFormat format,size_t layer) -> void
	{
		tex2D = Texture2D::create();
		tex2D->setName(name);
		tex2D->buildTexture(format, width, height, false, false, false);
		//PLOGE("use count when create texture {0}",tex2D.use_count());
	}
}