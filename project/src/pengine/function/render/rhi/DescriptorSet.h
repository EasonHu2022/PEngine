#pragma once
#include <string>
#include <vector>
#include <memory>
namespace pengine
{
	class Pipeline;
	class Shader;
	class Texture;
	class UniformBuffer;
	enum class TextureType : int32_t;
	enum class ShaderType : int32_t;
	enum class TextureFormat : int32_t;

	enum class DescriptorType
	{
		UniformBuffer,
		UniformBufferDynamic,
		ImageSampler,
		Image
	};

	enum class Format
	{
		R32G32B32A32_UINT,
		R32G32B32_UINT,
		R32G32_UINT,
		R32_UINT,
		R8_UINT,
		R32G32B32A32_INT,
		R32G32B32_INT,
		R32G32_INT,
		R32_INT,
		R32G32B32A32_FLOAT,
		R32G32B32_FLOAT,
		R32G32_FLOAT,
		R32_FLOAT
	};

	enum class ShaderDataType
	{
		None,
		Float32,
		Vec2,
		Vec3,
		Vec4,
		IVec2,
		IVec3,
		IVec4,
		Mat3,
		Mat4,
		Int32,
		Int,
		UInt,
		Bool,
		Struct,
		Mat4Array
	};

	struct BufferMemberInfo
	{
		uint32_t       size;
		uint32_t       offset;
		ShaderDataType type;
		std::string    name;
		std::string    fullName;
	};

	struct VertexInputDescription
	{
		uint32_t binding;
		uint32_t location;
		Format   format;
		uint32_t offset;
	};

	struct DescriptorPoolInfo
	{
		DescriptorType type;
		uint32_t       size;
	};

	struct DescriptorLayoutInfo
	{
		DescriptorType type;
		ShaderType     stage;
		uint32_t       binding = 0;
		uint32_t       setID = 0;
		uint32_t       count = 1;
	};

	struct DescriptorLayout
	{
		uint32_t              count;
		DescriptorLayoutInfo* layoutInfo;
	};

	struct DescriptorInfo
	{
		uint32_t layoutIndex;
		Shader* shader;
		uint32_t count = 1;      
	};

	struct Descriptor
	{
		std::vector<std::shared_ptr<Texture>> textures;
		std::shared_ptr<UniformBuffer>        buffer;

		uint32_t    offset;
		uint32_t    size;
		uint32_t    binding;
		std::string name;

		DescriptorType type = DescriptorType::ImageSampler;
		ShaderType     shaderType;

		TextureFormat format;
		uint32_t      accessFlag = 0;

		std::vector<BufferMemberInfo> members;
	};


	class DescriptorSet
	{
	public:
		virtual ~DescriptorSet() = default;
		static std::shared_ptr<DescriptorSet> create(const DescriptorInfo& desc) ;
		virtual void update() = 0;
		virtual void setDynamicOffset(uint32_t offset) = 0;
		virtual uint32_t getDynamicOffset() const = 0;
		virtual void setTexture(const std::string &name, const std::vector<std::shared_ptr<Texture>> & textures) = 0;
		virtual void setTexture(const std::string& name, const std::shared_ptr<Texture>& textures) = 0;
		virtual void setBuffer(const std::string& name, const std::shared_ptr<UniformBuffer>& buffer)   = 0;
		virtual std::shared_ptr<UniformBuffer>  getUnifromBuffer(const std::string& name) = 0;
		virtual void setUniform(const std::string& bufferName, const std::string& uniformName, const void* data, bool dynamic = false)  = 0;
		virtual void setUniform(const std::string& bufferName, const std::string& uniformName, const void* data, uint32_t size, bool dynamic = false)  = 0;
		virtual void setUniformBufferData(const std::string& bufferName, const void* data) = 0;
		virtual const std::vector<Descriptor>& getDescriptors() const = 0;

		static bool canUpdate();
		static void toggleUpdate(bool update);

	};


};