#pragma once
#include "core/core.h"
#include <function/render/rhi/DescriptorSet.h>
#include "function/resource/IResource.h"
namespace pengine
{
	class CommandBuffer;
	enum class ShaderType : int32_t
	{
		Vertex,
		Fragment,
		Geometry,
		TessellationControl,
		TessellationEvaluation,
		Compute,
		Unknown,
		Length
	};
	inline auto shaderTypeToName(ShaderType type) -> std::string
	{
		switch (type)
		{
#define STR(r)                 \
	case pengine::ShaderType::r: \
		return #r
			STR(Vertex);
			STR(Fragment);
			STR(Geometry);
			STR(TessellationControl);
			STR(TessellationEvaluation);
			STR(Compute);
#undef STR
		}
		return "Unknown";
	}
	struct Descriptor;
	struct PushConstant
	{
		uint32_t             size;
		ShaderType           shaderStage;
		std::vector<uint8_t> data;
		uint32_t             offset = 0;
		std::string          name;

		std::vector<BufferMemberInfo> members;

		inline auto setValue(const std::string& name, const void* value)
		{
			for (auto& member : members)
			{
				if (member.name == name)
				{
					memcpy(&data[member.offset], value, member.size);
					break;
				}
			}
		}

		inline auto setData(const void* value)
		{
			memcpy(data.data(), value, size);
		}
	};
	class Shader : public IResource
	{
	public:
		virtual ~Shader() = default;
		virtual auto bind() const -> void = 0;
		virtual auto unbind() const -> void = 0;
		virtual auto getName() const -> const std::string & = 0;
		virtual auto getFilePath() const -> const std::string & = 0;
		virtual auto getHandle() const -> void* = 0;
		virtual auto getPushConstants()->std::vector<PushConstant> & = 0;
		virtual auto bindPushConstants(CommandBuffer * commandBuffer, Pipeline * pipeline) -> void = 0;
		virtual auto getPushConstant(uint32_t index) -> PushConstant*
		{
			return nullptr;
		}

		virtual auto getDescriptorInfo(uint32_t index) -> const std::vector<Descriptor>
		{
			return  std::vector<Descriptor>{};
		}

		virtual auto getResourceType() const -> FileType
		{
			return FileType::Shader;
		}
		/*
		auto spirvTypeToDataType(const spirv_cross::SPIRType & type)->ShaderDataType;*/
		//auto spirvTypeToTextureType(const spv::ImageFormat & format)->TextureFormat;

		virtual auto reload() -> void {};

		inline auto getLocalSizeX() const
		{
			return localSizeX;
		};

		inline auto getLocalSizeY() const
		{
			return localSizeY;
		};
		inline auto getLocalSizeZ() const
		{
			return localSizeZ;
		};

		inline auto isComputeShader() const
		{
			return computeShader;
		}

	protected:
		auto parseSource(const std::vector<std::string> &lines, std::unordered_map<ShaderType, std::string> &shaders) -> void;

	public:
		static auto create(const std::string & filepath)->std::shared_ptr<Shader>;
		static auto create(const std::vector<uint32_t> &vertData, const std::vector<uint32_t> &fragData)->std::shared_ptr<Shader>;

	protected:
		bool     computeShader = false;
		uint32_t localSizeX = 1;
		uint32_t localSizeY = 1;
		uint32_t localSizeZ = 1;
	};
};