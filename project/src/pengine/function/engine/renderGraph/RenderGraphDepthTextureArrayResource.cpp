#include "RenderGraphDepthTextureArrayResource.h"
#include "function/render/rhi/CommandBuffer.h"
#include "function/render/rhi/Texture.h"
namespace pengine
{
	RenderGraphDepthTextureArrayResource::RenderGraphDepthTextureArrayResource()
	{
	}
	RenderGraphDepthTextureArrayResource::~RenderGraphDepthTextureArrayResource()
	{
		//PLOGV("texture use count in render graph resource : {0}", tex2D.use_count());
		depthArray.reset();
	}
	auto RenderGraphDepthTextureArrayResource::create(uint32_t width, uint32_t height,
		CommandBuffer* commandbuffer, const std::string& name, TextureFormat format, size_t layer) -> void
	{
		depthArray = TextureDepthArray::create(width,height,layer);
		depthArray->setName(name);
		//PLOGE("use count when create texture {0}",tex2D.use_count());
	}
}