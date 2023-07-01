#pragma once
#include "core/core.h"
#include <vector>
#include "function/resource/IResource.h"
#include "glm/glm.hpp"
namespace pengine
{
	class Texture2D;
	class Shader;
	class Pipeline;
	class UniformBuffer;
	class DescriptorSet;
	static constexpr float   PBR_WORKFLOW_SEPARATE_TEXTURES = 0.0f;
	static constexpr float   PBR_WORKFLOW_METALLIC_ROUGHNESS = 1.0f;
	static constexpr float   PBR_WORKFLOW_SPECULAR_GLOSINESS = 2.0f;
	static constexpr int32_t MATERAL_LAYOUT_INDEX = 1;
	static const std::string defaultTexture = "textures/default.png";
	//static const std::string defaultTexture = "F:/workspace/YizhouHu/PEngine/PEngine/assets/textures/default.png";
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
		auto loadPBRMaterial(const std::string& name, const std::string& path, const std::string& extension = ".png") -> void;
		auto loadMaterial(const std::string& name, const std::string& path) -> void;
		auto createDescriptorSet(int32_t layoutID = MATERAL_LAYOUT_INDEX, bool pbr = true) -> void;
		auto updateDescriptorSet() -> void;
		auto updateUniformBuffer() -> void;
		auto setMaterialProperites(const MaterialProperties& properties) -> void;
		auto setTextures(const PBRMataterialTextures& textures) -> void;
		auto setAlbedoTexture(const std::string& path) -> void;
		auto setAlbedo(const std::shared_ptr<Texture2D>& texture) -> void;
		auto setNormalTexture(const std::string& path) -> void;
		auto setRoughnessTexture(const std::string& path) -> void;
		auto setMetallicTexture(const std::string& path) -> void;
		auto setAOTexture(const std::string& path) -> void;
		auto setEmissiveTexture(const std::string& path) -> void;
		auto bind() -> void;
		inline auto setTexturesUpdated(bool updated)
		{
			texturesUpdated = updated;
		}

		inline auto setRenderFlags(int32_t flags)
		{
			renderFlags = flags;
		}

		inline auto setRenderFlag(Material::RenderFlags flag)
		{
			renderFlags |= static_cast<int32_t>(flag);
		}

		inline auto removeRenderFlag(Material::RenderFlags flag)
		{
			renderFlags &= ~static_cast<int32_t>(flag);
		}

		inline auto getShader() const
		{
			return shader;
		}

		inline auto& getName() const
		{
			return name;
		}

		inline const auto& getProperties() const
		{
			return materialProperties;
		}

		inline auto& getProperties()
		{
			return materialProperties;
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
		auto setShader(const std::string& path) -> void;
		auto setShader(const std::shared_ptr<Shader>& shader) -> void;
		auto getShaderPath() const->std::string;
		auto getResourceType() const->FileType { return FileType::Material; };
		auto getPath() const->std::string { return materialId; };
		static auto create(const std::string& materialId) -> std::shared_ptr<Material>;
		Material(const std::string& materialId);
	private:
		PBRMataterialTextures pbrMaterialTextures;
		MaterialProperties    materialProperties;

		std::shared_ptr<Shader>        shader;
		std::shared_ptr<UniformBuffer> materialPropertiesBuffer;
		std::string                    name;
		std::string                    materialId;
		std::shared_ptr<DescriptorSet> descriptorSet;
		bool texturesUpdated = false;
	protected:
		int32_t renderFlags = 0;

	};

}

