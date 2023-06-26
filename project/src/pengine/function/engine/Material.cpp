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

	auto Material::bind() -> void
	{
	}

};