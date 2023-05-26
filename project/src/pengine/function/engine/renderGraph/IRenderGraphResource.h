#pragma once
#include <string>
#include <stdint.h>
#include "function/render/rhi/Defs.h"
namespace pengine
{
	
	enum RenderResouceType
	{
		Res_Texture2D = 0,
		Res_TextureDepth = 1,
		Res_TextureCube = 2,
		Res_Buffer = 3,
		
	};
	//just define the count and bind pos of each tast
	//don't hold any real res;
	//hold an index of the real res tabl
	class RenderGraphVirtualResource
	{
	public:
		//maybe type
		RenderResouceType type;
		//index in the real render res table
		uint32_t index;

		TextureFormat format = TextureFormat::RGBA16;
		const std::string name = " ";
		uint32_t width;
		uint32_t height;
		
	};
	class CommandBuffer;
	class IRenderGraphResource
	{
	public:
		//hold the handle to the real res
		virtual auto create(uint32_t width, uint32_t height, CommandBuffer* commandbuffer, const std::string& name, TextureFormat format) -> void = 0;
		
	};
};