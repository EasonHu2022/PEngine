#pragma once
#include "core/core.h"
#include "function/render/rhi/DescriptorSet.h"
#include "function/engine/Buffer.h"
#include "VKHelper.h"

#include <unordered_map>
#include <array>
namespace pengine
{
	constexpr int32_t MAX_BUFFER_INFOS = 32;
	constexpr int32_t MAX_IMAGE_INFOS = 32;
	constexpr int32_t MAX_WRITE_DESCTIPTORS = 32;
	class VulkanDescriptorSet final : public DescriptorSet
	{
	public:
		VulkanDescriptorSet(const DescriptorInfo &info);
		~VulkanDescriptorSet();
		NO_COPYABLE(VulkanDescriptorSet);

		void update() override;
		inline void setDynamicOffset(uint32_t offset) override
		{
			dynamicOffset = offset;
		}
		inline uint32_t getDynamicOffset() const override
		{
			return dynamicOffset;
		}

		inline auto isDynamic() const
		{
			return dynamic;
		}

		VkDescriptorSet getDescriptorSet();

		void setTexture(const std::string& name, const std::vector<std::shared_ptr<Texture>>& textures)  override;
		void setTexture(const std::string& name, const std::shared_ptr<Texture>& textures)  override;
		void setBuffer(const std::string& name, const std::shared_ptr<UniformBuffer>& buffer)  override;
		std::shared_ptr<UniformBuffer> getUnifromBuffer(const std::string& name) override;
		void setUniform(const std::string& bufferName, const std::string& uniformName, const void* data, bool dynamic)  override;
		void setUniform(const std::string& bufferName, const std::string& uniformName, const void* data, uint32_t size, bool dynamic)  override;
		void setUniformBufferData(const std::string& bufferName, const void* data)  override;
		const std::vector<Descriptor>& getDescriptors() const  override { return descriptors; }
	private:
		uint32_t dynamicOffset = 0;
		Shader* shader = nullptr;
		bool     dynamic = false;
		bool     descriptorDirty[3] = {};

		std::vector<Descriptor> descriptors;

		std::array<VkDescriptorBufferInfo, MAX_BUFFER_INFOS>    bufferInfoPool;
		std::array<VkDescriptorImageInfo, MAX_IMAGE_INFOS>      imageInfoPool;
		std::array<VkWriteDescriptorSet, MAX_WRITE_DESCTIPTORS> writeDescriptorSetPool;

		uint32_t framesInFlight = 0;

		struct UniformBufferInfo
		{
			std::vector<BufferMemberInfo> members;
			Buffer                        localStorage;
			bool                          dynamic = false;
			bool                          hasUpdated[10] = {};
		};

		std::vector<VkDescriptorSet>                                                 descriptorSet;
		std::vector<std::unordered_map<std::string, std::shared_ptr<UniformBuffer>>> uniformBuffers;
		std::unordered_map<std::string, UniformBufferInfo>                           uniformBuffersData;
	};

	
};