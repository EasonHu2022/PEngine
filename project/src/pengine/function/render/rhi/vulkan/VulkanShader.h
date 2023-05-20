#pragma once
#include "VKHelper.h"
#include "function/render/rhi/Shader.h"
namespace pengine
{
	class VulkanShader : public Shader
	{
	public:
		VulkanShader(const std::string& path);
		VulkanShader(const std::vector<uint32_t>& vertData, const std::vector<uint32_t>& fragData);
		~VulkanShader();
		NO_COPYABLE(VulkanShader);
		auto bindPushConstants(CommandBuffer* commandBuffer, Pipeline* pipeline) -> void override;

		auto bind() const -> void override {};
		auto unbind() const -> void override {};

		inline auto getName() const -> const std::string & override
		{
			return name;
		}
		inline auto getFilePath() const -> const std::string & override
		{
			return filePath;
		}

		inline auto getPath() const -> std::string override
		{
			return filePath;
		}

		inline auto getHandle() const -> void* override
		{
			return nullptr;
		}

		inline auto getPushConstants() -> std::vector<PushConstant> & override
		{
			return pushConstants;
		};

		inline auto getPushConstant(uint32_t index) -> PushConstant* override
		{
			if (index < pushConstants.size())
			{
				return &pushConstants[index];
			}
			return nullptr;
		}

		inline auto getDescriptorInfo(uint32_t index) -> const std::vector<Descriptor> override
		{
			if (descriptorInfos.find(index) != descriptorInfos.end())
			{
				return descriptorInfos[index];
			}

			PLOGE("DescriptorDesc not found. Index = {0}", index);
			return std::vector<Descriptor>{};
		}

		inline auto getDescriptorLayout(uint32_t index) const
		{
			return &descriptorSetLayouts[index];
		}

		inline auto& getPipelineLayout() const
		{
			return pipelineLayout;
		}

		inline auto getVertexInputStride() const
		{
			return vertexInputStride;
		}

		inline auto& getVertexInputAttributeDescription() const
		{
			return vertexInputAttributeDescriptions;
		}

		inline auto& getShaderStages() const
		{
			return shaderStages;
		}

	private:
		auto loadShader(const std::vector<uint32_t>& spvCode, ShaderType type, int32_t currentShaderStage) -> void;
		auto init() -> void;
		auto createPipelineLayout() -> void;
		auto unload() const -> void;

		VkPipelineLayout pipelineLayout;

		std::vector<VkPipelineShaderStageCreateInfo> shaderStages;

		uint32_t vertexInputStride = 0;

		std::string name;
		std::string filePath;
		std::string source;

		std::vector<ShaderType>                         shaderTypes;
		std::vector<PushConstant>                       pushConstants;
		std::vector<DescriptorLayoutInfo>               descriptorLayoutInfo;
		std::vector<VkDescriptorSetLayout>              descriptorSetLayouts;
		std::vector<VkVertexInputAttributeDescription>  vertexInputAttributeDescriptions;
		std::unordered_map<uint32_t, std::vector<Descriptor>> descriptorInfos;
	};

	
};