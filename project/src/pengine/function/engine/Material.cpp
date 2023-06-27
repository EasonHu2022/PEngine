#include "Material.h"
#include "function/render/rhi/Texture.h"
#include "function/render/rhi/Shader.h"
namespace pengine
{



	Material::Material(const std::shared_ptr<Shader>& shader, const MaterialProperties& properties, const PBRMataterialTextures& textures)
	{
	}

	Material::Material()
	{
	}

	Material::~Material()
	{
	}

	auto Material::loadMaterial(const std::string& name, const std::string& path) -> void
	{
	}

	auto Material::createDescriptorSet(int32_t layoutID, bool pbr) -> void
	{
	}

	auto Material::updateDescriptorSet() -> void
	{
	}

	auto Material::updateUniformBuffer() -> void
	{
	}

	auto Material::setMaterialProperites(const MaterialProperties& properties) -> void
	{
	}

	auto Material::setTextures(const PBRMataterialTextures& textures) -> void
	{
	}

	auto Material::setAlbedoTexture(const std::string& path) -> void
	{
	}

	auto Material::setAlbedo(const std::shared_ptr<Texture2D>& texture) -> void
	{
	}

	auto Material::setNormalTexture(const std::string& path) -> void
	{
	}

	auto Material::setRoughnessTexture(const std::string& path) -> void
	{
	}

	auto Material::setMetallicTexture(const std::string& path) -> void
	{
	}

	auto Material::setAOTexture(const std::string& path) -> void
	{
	}

	auto Material::setEmissiveTexture(const std::string& path) -> void
	{
	}

	auto Material::bind() -> void
	{
	}

};