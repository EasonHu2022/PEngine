#include "OBJLoader.h"
#include "function/render/rhi/Texture.h"
#include "core/utils/StringUtils.h"
#include "function/engine/Material.h"
#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"
namespace pengine
{
    std::vector<std::shared_ptr<Texture2D>> texturesCache;

	std::shared_ptr<Texture2D> loadMaterialTextures(const std::string& typeName,
		std::vector<std::shared_ptr<Texture2D>>& texturesLoaded, const std::string& name, 
		const std::string& directory, TextureParameters format)
	{
		for (uint32_t j = 0; j < texturesLoaded.size(); j++)
		{
			if (std::strcmp(texturesLoaded[j]->getFilePath().c_str(), (directory + "/" + name).c_str()) == 0)
			{
				return texturesLoaded[j];
			}
		}

		{   
			// If texture hasn't been loaded already, load it
			TextureLoadOptions options(false, true);
			auto               texture = Texture2D::create(typeName, directory + "/" + name, format, { false,false,false });
			texturesLoaded.push_back(texture);        // Store it as texture loaded for entire model, to ensure we won't unnecessary load duplicate textures.
			return texture;
		}
	}

    auto OBJLoader::load(const std::string& obj, const std::string& extension, std::unordered_map<std::string, std::shared_ptr<Mesh>>& meshes, std::shared_ptr<Skeleton>& skeleton) -> void
    {
		std::string resolvedPath = obj;
		auto        directory = resolvedPath.substr(0, resolvedPath.find_last_of(StringUtils::delimiter));
		//could change : set parent directory as name also can 
		std::string name = directory.substr(directory.find_last_of(StringUtils::delimiter) + 1);

		tinyobj::attrib_t                attrib;
		std::vector<tinyobj::shape_t>    shapes;
		std::vector<tinyobj::material_t> materials;
		std::string                      warn, err;

		if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, obj.c_str(), directory.c_str()))
		{
			throw std::runtime_error(warn + err);
		}

		for (const auto& shape : shapes)
		{
			std::vector<Vertex>                  vertices;
			std::vector<uint32_t>                indices;
			std::unordered_map<Vertex, uint32_t> uniqueVertices{};

			for (const auto& index : shape.mesh.indices)
			{
				Vertex vertex{};

				vertex.pos = {
					attrib.vertices[3 * index.vertex_index + 0],
					attrib.vertices[3 * index.vertex_index + 1],
					attrib.vertices[3 * index.vertex_index + 2] };

				if (index.normal_index >= 0)
					vertex.normal = {
						attrib.normals[3 * index.normal_index + 0],
						attrib.normals[3 * index.normal_index + 1],
						attrib.normals[3 * index.normal_index + 2] };

				if (index.texcoord_index >= 0)
					vertex.texCoord = {
						attrib.texcoords[2 * index.texcoord_index + 0],
						1.0f - attrib.texcoords[2 * index.texcoord_index + 1] };

				vertex.color = { 1.0f, 1.0f, 1.0f, 1.f };

				if (uniqueVertices.count(vertex) == 0)
				{
					uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
					vertices.push_back(vertex);
				}

				indices.emplace_back(uniqueVertices[vertex]);
			}

			if (attrib.normals.empty())
				Mesh::generateNormals(vertices, indices);

			Mesh::generateTangents(vertices, indices);

			auto pbrMaterial = std::make_shared<Material>();

			PBRMataterialTextures textures;

			if (shape.mesh.material_ids[0] >= 0)
			{
				tinyobj::material_t* mp = &materials[shape.mesh.material_ids[0]];

				if (mp->diffuse_texname.length() > 0)
				{
					std::shared_ptr<Texture2D> texture = loadMaterialTextures("Albedo",
						texturesCache,
						mp->diffuse_texname, directory, TextureParameters(TextureFilter::Nearest, TextureFilter::Nearest, mp->diffuse_texopt.clamp ? TextureWrap::ClampToEdge : TextureWrap::Repeat));
					if (texture)
						textures.albedo = texture;
				}

				if (mp->bump_texname.length() > 0)
				{
					std::shared_ptr<Texture2D> texture = loadMaterialTextures("Normal", texturesCache, mp->bump_texname, directory, TextureParameters(TextureFilter::Nearest, TextureFilter::Nearest, mp->bump_texopt.clamp ? TextureWrap::ClampToEdge : TextureWrap::Repeat));
					if (texture)
						textures.normal = texture;        //pbrMaterial->SetNormalMap(texture);
				}

				if (mp->roughness_texname.length() > 0)
				{
					std::shared_ptr<Texture2D> texture = loadMaterialTextures("Roughness", texturesCache, mp->roughness_texname.c_str(), directory, TextureParameters(TextureFilter::Nearest, TextureFilter::Nearest, mp->roughness_texopt.clamp ? TextureWrap::ClampToEdge : TextureWrap::Repeat));
					if (texture)
						textures.roughness = texture;
				}

				if (mp->metallic_texname.length() > 0)
				{
					std::shared_ptr<Texture2D> texture = loadMaterialTextures("Metallic", texturesCache, mp->metallic_texname, directory, TextureParameters(TextureFilter::Nearest, TextureFilter::Nearest, mp->metallic_texopt.clamp ? TextureWrap::ClampToEdge : TextureWrap::Repeat));
					if (texture)
						textures.metallic = texture;
				}

				if (mp->specular_highlight_texname.length() > 0)
				{
					std::shared_ptr<Texture2D> texture = loadMaterialTextures("Metallic", texturesCache, mp->specular_highlight_texname, directory, TextureParameters(TextureFilter::Nearest, TextureFilter::Nearest, mp->specular_texopt.clamp ? TextureWrap::ClampToEdge : TextureWrap::Repeat));
					if (texture)
						textures.metallic = texture;
				}
			}
			pbrMaterial->setTextures(textures);
			auto mesh = std::make_shared<Mesh>(indices, vertices);
			mesh->setMaterial(pbrMaterial);
			mesh->setName(shape.name);
			meshes.emplace(shape.name, mesh);
		}
    }
};