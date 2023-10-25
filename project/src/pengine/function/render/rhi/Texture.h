#pragma once
#include "core/core.h"
#include "Defs.h"
#include "function/resource/IResource.h"
namespace pengine
{
	class PENGINE_API Texture : public IResource
	{
	public:
		virtual ~Texture()
		{
		}

		virtual auto bind(uint32_t slot = 0) const -> void = 0;
		virtual auto unbind(uint32_t slot = 0) const -> void = 0;
		virtual auto getFilePath() const -> const std::string & = 0;
		virtual auto getHandle() const -> void* = 0;
		virtual auto getWidth() const->uint32_t = 0;
		virtual auto getHeight() const->uint32_t = 0;
		virtual auto getDepth() const->uint32_t = 0;
		virtual auto getType() const->TextureType = 0;
		virtual auto getFormat() const->TextureFormat = 0;

		virtual auto bindImageTexture(uint32_t unit, bool read = false, bool write = false, uint32_t level = 0, uint32_t layer = 0) -> void {};
		virtual auto getSize() const -> uint32_t
		{
			return 0;
		}
		virtual auto getMipMapLevels() const -> uint32_t
		{
			return 0;
		}

		virtual auto getDescriptorInfo() const -> void*
		{
			return getHandle();
		}

		inline static auto isDepthStencilFormat(TextureFormat format)
		{
			return format == TextureFormat::DEPTH_STENCIL;
		}

		inline static auto isDepthFormat(TextureFormat format)
		{
			return format == TextureFormat::DEPTH;
		}

		inline static bool isStencilFormat(TextureFormat format)
		{
			return format == TextureFormat::STENCIL;
		}

		virtual auto setName(const std::string& name) -> void
		{
			this->name = name;
		};

		virtual auto getName() const -> const std::string&
		{
			return name;
		};

		virtual auto getResourceType() const -> FileType override
		{
			return FileType::Texture;
		};

		virtual auto getPath() const -> std::string override
		{
			return "";
		}

	public:
		static auto getStrideFromFormat(TextureFormat format) -> uint8_t;
		static auto bitsToTextureFormat(uint32_t bits) -> TextureFormat;
		static auto calculateMipMapCount(uint32_t width, uint32_t height) -> uint32_t;

	protected:
		uint16_t    flags = 0;
		std::string name;
	};

	class PENGINE_API Texture2D : public Texture
	{
	public:
		virtual auto setData(const void* data) -> void = 0;

	public:
		static auto  getDefaultTexture() -> std::shared_ptr<Texture2D>;
		static auto  create() -> std::shared_ptr<Texture2D>;
		static auto  create(uint32_t width, uint32_t height, void* data, TextureParameters parameters = TextureParameters(), TextureLoadOptions loadOptions = TextureLoadOptions()) -> std::shared_ptr<Texture2D>;
		static auto  create(const std::string& name, const std::string& filePath, TextureParameters parameters = TextureParameters(), TextureLoadOptions loadOptions = TextureLoadOptions()) -> std::shared_ptr<Texture2D>;
		virtual auto update(int32_t x, int32_t y, int32_t w, int32_t h, const void* buffer) -> void = 0;

		virtual auto buildTexture(TextureFormat internalformat, uint32_t width, uint32_t height, bool srgb = false, bool depth = false, bool samplerShadow = false, bool mipmap = false, bool image = false, uint32_t accessFlag = 0) -> void = 0;

		inline auto getType() const -> TextureType override
		{
			return TextureType::Color;
		};
	};

	class PENGINE_API Texture3D : public Texture2D
	{
	public:
		static auto create(uint32_t width, uint32_t height, uint32_t depth, TextureParameters parameters = {}, TextureLoadOptions loadOptions = {}) -> std::shared_ptr<Texture3D>;

		virtual auto init() -> void = 0;
		virtual auto update(int32_t x, int32_t y, int32_t w, int32_t h, const void* buffer) -> void
		{}

		virtual auto buildTexture(TextureFormat internalformat, uint32_t width, uint32_t height, bool srgb = false, bool depth = false, bool samplerShadow = false, bool mipmap = false, bool image = false, uint32_t accessFlag = 0) -> void
		{}

		virtual auto buildTexture3D(TextureFormat format, uint32_t width, uint32_t height, uint32_t depth) -> void
		{}

		virtual auto setData(const void* data) -> void override
		{}

		virtual auto generateMipmaps() -> void = 0;

		inline auto getType() const -> TextureType override
		{
			return TextureType::Color3D;
		};

		virtual auto clear() -> void {};
	};

	class PENGINE_API TextureCube : public Texture
	{
	protected:
		enum class InputFormat
		{
			VERTICAL_CROSS,
			HORIZONTAL_CROSS
		};

	public:
		static auto create(uint32_t size) -> std::shared_ptr<TextureCube>;
		static auto create(uint32_t size, TextureFormat format, int32_t numMips) -> std::shared_ptr<TextureCube>;
		static auto createFromFile(const std::string& filePath) -> std::shared_ptr<TextureCube>;
		static auto createFromFiles(const std::array<std::string, 6>& files) -> std::shared_ptr<TextureCube>;
		static auto createFromVCross(const std::vector<std::string>& files, uint32_t mips, TextureParameters params, TextureLoadOptions loadOptions, InputFormat = InputFormat::VERTICAL_CROSS) -> std::shared_ptr<TextureCube>;

		virtual auto update(CommandBuffer* commandBuffer, FrameBuffer* framebuffer, int32_t cubeIndex, int32_t mipmapLevel = 0) -> void = 0;

		virtual auto generateMipmap(const CommandBuffer* commandBuffer) -> void = 0;
		inline auto  getType() const -> TextureType override
		{
			return TextureType::Cube;
		};
	};

	class PENGINE_API TextureDepth : public Texture
	{
	public:
		static auto create(uint32_t width, uint32_t height, bool stencil = false) -> std::shared_ptr<TextureDepth>;
		inline auto getType() const -> TextureType override
		{
			return TextureType::Depth;
		};
		virtual auto resize(uint32_t width, uint32_t height, CommandBuffer* commandBuffer = nullptr) -> void = 0;
	};

	class PENGINE_API TextureDepthArray : public Texture
	{
	public:
		static auto create(uint32_t width, uint32_t height, uint32_t count) -> std::shared_ptr<TextureDepthArray>;

		virtual auto init() -> void = 0;
		virtual auto resize(uint32_t width, uint32_t height, uint32_t count) -> void = 0;
		virtual auto getHandleArray(uint32_t index) -> void*
		{
			return getHandle();
		};
		inline auto getType() const -> TextureType override
		{
			return TextureType::DepthArray;
		};
	};


};