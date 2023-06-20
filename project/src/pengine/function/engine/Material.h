#pragma once
#include "core/core.h"
#include <vector>
#include "function/resource/IResource.h"
namespace pengine
{
	class Texture2D;
	class Shader;
	class Pipeline;
	class UniformBuffer;
	class DescriptorSet;

	static constexpr int32_t MATERAL_LAYOUT_INDEX = 1;
	struct MaterialProperties
	{
		glm::vec4 albedoColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
		glm::vec4 roughnessColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
		glm::vec4 metallicColor = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
		glm::vec4 emissiveColor = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
		float     usingAlbedoMap = 1.0f;
		float     usingMetallicMap = 1.0f;
		float     usingRoughnessMap = 1.0f;
		float     usingNormalMap = 1.0f;
		float     usingAOMap = 1.0f;
		float     usingEmissiveMap = 1.0f;
		float     workflow = PBR_WORKFLOW_SEPARATE_TEXTURES;

		//padding in vulkan
		float padding = 0.0f;
	};

	struct PBRMataterialTextures
	{
		std::shared_ptr<Texture2D> albedo;
		std::shared_ptr<Texture2D> normal;
		std::shared_ptr<Texture2D> metallic;
		std::shared_ptr<Texture2D> roughness;
		std::shared_ptr<Texture2D> ao;
		std::shared_ptr<Texture2D> emissive;
	};
	class Material : public IResource
	{
	public:
		enum class RenderFlags : uint32_t
		{
			NONE = 0,
			DepthTest = BIT(0),
			Wireframe = BIT(1),
			ForwardRender = BIT(2),
			DeferredRender = BIT(3),
			ForwardPreviewRender = BIT(4),
			NoShadow = BIT(5),
			TwoSided = BIT(6),
			AlphaBlend = BIT(7)
		};
	public:
		Material(const std::shared_ptr<Shader>& shader, const MaterialProperties& properties = MaterialProperties(), const PBRMataterialTextures& textures = PBRMataterialTextures());
		Material();
		~Material();
		auto loadMaterial(const std::string& name, const std::string& path) -> void;
		auto createDescriptorSet(int32_t layoutID = MATERAL_LAYOUT_INDEX, bool pbr = true) -> void;
		auto updateDescriptorSet() -> void;
		auto updateUniformBuffer() -> void;

		auto bind() -> void;

		inline auto getShader() const
		{
			return shader;
		}

		inline auto& getName() const
		{
			return name;
		}

		inline auto& getMaterialId() const
		{
			return materialId;
		}

		inline auto getDescriptorSet()
		{
			return descriptorSet;
		}

		inline auto getRenderFlags() const
		{
			return renderFlags;
		}

		inline auto isFlagOf(RenderFlags flag) const -> bool
		{
			return (uint32_t)flag & renderFlags;
		}
	private:
		std::shared_ptr<Shader>        shader;
		std::shared_ptr<UniformBuffer> materialPropertiesBuffer;
		std::string                    name;
		std::string                    materialId;
		std::shared_ptr<DescriptorSet> descriptorSet;
	protected:
		int32_t renderFlags = 0;

	};

}

