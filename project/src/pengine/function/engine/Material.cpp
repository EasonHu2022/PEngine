#include "Material.h"
#include "Application.h"
#include "core/utils/StringUtils.h"
#include "function/render/rhi/Texture.h"
#include "function/render/rhi/Shader.h"
namespace pengine
{

	Material::Material(const std::shared_ptr<Shader>& shader, const MaterialProperties& properties, const PBRMataterialTextures& textures) :
		pbrMaterialTextures(textures), shader(shader)
	{


		setRenderFlag(RenderFlags::DepthTest);
		setRenderFlag(RenderFlags::DeferredRender);
		setMaterialProperites(properties);
	}

	Material::Material()
	{

		setRenderFlag(RenderFlags::DepthTest);
		setRenderFlag(RenderFlags::DeferredRender);
	}

	Material::Material(const std::string& materialId) :
		materialId(materialId)
	{
		setRenderFlag(RenderFlags::DepthTest);
	}

	Material::~Material()
	{
	}

	auto Material::loadPBRMaterial(const std::string& name, const std::string& path, const std::string& extension) -> void
	{
	
	}

	auto Material::loadMaterial(const std::string& name, const std::string& path) -> void
	{
		this->name = name;
		pbrMaterialTextures.albedo = Texture2D::create(name, path);
		pbrMaterialTextures.normal = nullptr;
		pbrMaterialTextures.roughness = nullptr;
		pbrMaterialTextures.metallic = nullptr;
		pbrMaterialTextures.ao = nullptr;
		pbrMaterialTextures.emissive = nullptr;
	}

	auto Material::createDescriptorSet(int32_t layoutID, bool pbr) -> void
	{

		if (shader == nullptr)
		{
			if (isFlagOf(RenderFlags::ForwardRender))
			{
				std::string path = "shaders/ForwardPBR.shader";
				shader = Shader::create(ASSETS_ABSOLUTE_PATH + path);
			}

			if (isFlagOf(RenderFlags::DeferredRender))
			{
				//shader = Shader::create("F:/workspace/YizhouHu/PEngine/PEngine/assets/shaders/DeferredColor.shader");
				std::string path = "shaders/DeferredColor.shader";
				shader = Shader::create(ASSETS_ABSOLUTE_PATH + path);
			}

			if (isFlagOf(RenderFlags::ForwardPreviewRender))
			{
				std::string path = "shaders/DeferredColor.shader";
				shader = Shader::create(ASSETS_ABSOLUTE_PATH + path);
			}
		}

		DescriptorInfo descriptorDesc;
		descriptorDesc.layoutIndex = layoutID;
		descriptorDesc.shader = shader.get();

		descriptorSet = DescriptorSet::create(descriptorDesc);
		updateDescriptorSet();
	}

	auto Material::updateDescriptorSet() -> void
	{

		descriptorSet->setTexture("uAlbedoMap", pbrMaterialTextures.albedo);
		descriptorSet->setTexture("uMetallicMap", pbrMaterialTextures.metallic);
		descriptorSet->setTexture("uRoughnessMap", pbrMaterialTextures.roughness);
		descriptorSet->setTexture("uNormalMap", pbrMaterialTextures.normal);
		descriptorSet->setTexture("uAOMap", pbrMaterialTextures.ao);
		descriptorSet->setTexture("uEmissiveMap", pbrMaterialTextures.emissive);

		if (pbrMaterialTextures.albedo != nullptr)
		{
			descriptorSet->setTexture("uAlbedoMap", pbrMaterialTextures.albedo);
		}
		else
		{
			descriptorSet->setTexture("uAlbedoMap", Texture2D::create("albedo", ASSETS_ABSOLUTE_PATH + defaultTexture));
			materialProperties.usingAlbedoMap = 0.0f;
		}

		if (pbrMaterialTextures.metallic)
		{
			descriptorSet->setTexture("uMetallicMap", pbrMaterialTextures.metallic);
		}
		else
		{
			materialProperties.usingMetallicMap = 0.0f;
			descriptorSet->setTexture("uMetallicMap", Texture2D::create("metallic", ASSETS_ABSOLUTE_PATH + defaultTexture));
		}

		if (pbrMaterialTextures.roughness)
		{
			descriptorSet->setTexture("uRoughnessMap", pbrMaterialTextures.roughness);
		}
		else
		{
			descriptorSet->setTexture("uRoughnessMap", Texture2D::create("roughness", ASSETS_ABSOLUTE_PATH + defaultTexture));
			materialProperties.usingRoughnessMap = 0.0f;
		}

		if (pbrMaterialTextures.normal != nullptr)
		{
			descriptorSet->setTexture("uNormalMap", pbrMaterialTextures.normal);
		}
		else
		{
			descriptorSet->setTexture("uNormalMap", Texture2D::create("normal", ASSETS_ABSOLUTE_PATH + defaultTexture));
			materialProperties.usingNormalMap = 0.0f;
		}

		if (pbrMaterialTextures.ao != nullptr)
		{
			descriptorSet->setTexture("uAOMap", pbrMaterialTextures.ao);
		}
		else
		{
			descriptorSet->setTexture("uAOMap", Texture2D::create("ao", ASSETS_ABSOLUTE_PATH + defaultTexture));
			materialProperties.usingAOMap = 0.0f;
		}

		if (pbrMaterialTextures.emissive != nullptr)
		{
			descriptorSet->setTexture("uEmissiveMap", pbrMaterialTextures.emissive);
		}
		else
		{
			descriptorSet->setTexture("uEmissiveMap", Texture2D::create("emissive", ASSETS_ABSOLUTE_PATH + defaultTexture));
			materialProperties.usingEmissiveMap = 0.0f;
		}

		updateUniformBuffer();
	}

	auto Material::updateUniformBuffer() -> void
	{

		if (!descriptorSet)
			return;

		descriptorSet->setUniformBufferData("UniformMaterialData", &materialProperties);
	}

	auto Material::setMaterialProperites(const MaterialProperties& properties) -> void
	{

		materialProperties = properties;
		updateUniformBuffer();
	}

	auto Material::setTextures(const PBRMataterialTextures& textures) -> void
	{

		pbrMaterialTextures = textures;
	}

	auto Material::setAlbedoTexture(const std::string& path) -> void
	{
		setAlbedo(Texture2D::create(path, path));
	}

	auto Material::setAlbedo(const std::shared_ptr<Texture2D>& texture) -> void
	{

		if (texture)
		{
			pbrMaterialTextures.albedo = texture;
			materialProperties.usingAlbedoMap = 1.0;
			texturesUpdated = true;
		}
	}

	auto Material::setNormalTexture(const std::string& path) -> void
	{

		auto tex = Texture2D::create(path, path);
		if (tex)
		{
			pbrMaterialTextures.normal = tex;
			materialProperties.usingNormalMap = 1.0;
			texturesUpdated = true;
		}
	}

	auto Material::setRoughnessTexture(const std::string& path) -> void
	{
	
		auto tex = Texture2D::create(path, path);
		if (tex)
		{
			pbrMaterialTextures.roughness = tex;
			materialProperties.usingRoughnessMap = 1.0;

			texturesUpdated = true;
		}
	}

	auto Material::setMetallicTexture(const std::string& path) -> void
	{

		auto tex = Texture2D::create(path, path);
		if (tex)
		{
			pbrMaterialTextures.metallic = tex;
			materialProperties.usingMetallicMap = 1.0;

			texturesUpdated = true;
		}
	}

	auto Material::setAOTexture(const std::string& path) -> void
	{

		auto tex = Texture2D::create(path, path);
		if (tex)
		{
			pbrMaterialTextures.ao = tex;
			materialProperties.usingAOMap = 1.0;
			texturesUpdated = true;
		}
	}

	auto Material::setEmissiveTexture(const std::string& path) -> void
	{

		auto tex = Texture2D::create(path, path);
		if (tex)
		{
			pbrMaterialTextures.emissive = tex;
			materialProperties.usingEmissiveMap = 1.0;
			texturesUpdated = true;
		}
	}

	auto Material::bind() -> void
	{


		if (descriptorSet == nullptr)
		{
			createDescriptorSet();
			setTexturesUpdated(false);
		}
		if (texturesUpdated)
		{
			updateDescriptorSet();
		}
		descriptorSet->update();
	}

	auto Material::setShader(const std::string& path) -> void
	{
		if (!StringUtils::endWith(path, "ForwardPreview.shader"))
		{
			shader = Shader::create(path);
		}
	}

	auto Material::setShader(const std::shared_ptr<Shader>& shader) -> void
	{

		this->shader = shader;
	}

	auto Material::getShaderPath() const -> std::string
	{
		return shader ? shader->getFilePath() : "";
	}

	auto Material::create(const std::string& materialId) -> std::shared_ptr<Material>
	{
		return Application::getCache()->emplace<Material>(materialId);
	}


};