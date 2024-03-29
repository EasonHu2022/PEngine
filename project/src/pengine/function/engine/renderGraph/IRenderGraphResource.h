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
		Res_DepthArray = 4,
	};
	//just define the count and bind pos of each tast
	//don't hold any real res;
	//hold an index of the real res tabl
	class RenderGraphVirtualResource
	{
	public:
		void onResize(float w, float h) {
			width = w;
			height = h;
			b_initialized = false;
		};
		//maybe type
		RenderResouceType type;
		//index in the real render res table
		uint32_t index = -1;

		TextureFormat format = TextureFormat::RGBA16;
		const std::string name = " ";
		uint32_t width;
		uint32_t height;
		size_t layer = 1;
		bool b_initialized = false;
	};
	class CommandBuffer;
	class IRenderGraphResource
	{
	public:
		//hold the handle to the real graphics res
		virtual auto create(uint32_t width, uint32_t height, CommandBuffer* commandbuffer, const std::string& name, TextureFormat format, size_t layer = 1) -> void = 0;
		virtual auto getNativeResource() -> std::shared_ptr<Texture> = 0;
	};
};