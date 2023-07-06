#include "FBXLoader.h"
#include "core/core.h"
#include "function/ecs/component/Transform.h"
#include "function/render/rhi/Texture.h"
#include "File.h"
#include "function/engine/Material.h"
#include "function/engine/Skeleton.h"
#include "function/engine/BoundingBox.h"
#include "mio.hpp"
#pragma push_macro("VOID")
#undef VOID
#include "ofbx.h"
#pragma pop_macro("VOID")
namespace pengine
{
	enum class Orientation
	{
		Y_UP,
		Z_UP,
		Z_MINUS_UP,
		X_MINUS_UP,
		X_UP
	};
	namespace
	{
		static auto operator-(const ofbx::Vec3& a, const ofbx::Vec3& b) -> ofbx::Vec3
		{
			return { a.x - b.x, a.y - b.y, a.z - b.z };
		}

		static auto operator-(const ofbx::Vec2& a, const ofbx::Vec2& b) -> ofbx::Vec2
		{
			return { a.x - b.x, a.y - b.y };
		}

		inline auto toGlm(const ofbx::Vec2& vec)
		{
			return glm::vec2(float(vec.x), float(vec.y));
		}

		inline auto toGlm(const ofbx::Vec3& vec)
		{
			return glm::vec3(float(vec.x), float(vec.y), float(vec.z));
		}

		inline auto toGlm(const ofbx::Vec4& vec)
		{
			return glm::vec4(float(vec.x), float(vec.y), float(vec.z), float(vec.w));
		}

		inline auto toGlm(const ofbx::Color& vec)
		{
			return glm::vec4(float(vec.r), float(vec.g), float(vec.b), 1.0f);
		}

		inline auto fixOrientation(const glm::vec3& v, Orientation orientation) -> glm::vec3
		{
			switch (orientation)
			{
			case Orientation::Y_UP:
				return glm::vec3(v.x, v.y, v.z);
			case Orientation::Z_UP:
				return glm::vec3(v.x, v.z, -v.y);
			case Orientation::Z_MINUS_UP:
				return glm::vec3(v.x, -v.z, v.y);
			case Orientation::X_MINUS_UP:
				return glm::vec3(v.y, -v.x, v.z);
			case Orientation::X_UP:
				return glm::vec3(-v.y, v.x, v.z);
			}
			return v;
		}

		inline auto fixOrientation(const glm::quat& v, Orientation orientation) -> glm::quat
		{
			switch (orientation)
			{
			case Orientation::Y_UP:
				return { v.w,v.x, v.y, v.z };
			case Orientation::Z_UP:
				return { v.w,v.x,-v.y, v.z };
			case Orientation::Z_MINUS_UP:
				return { v.w,v.x, -v.z, v.y };
			case Orientation::X_MINUS_UP:
				return { v.w, -v.x, v.z, v.y };
			case Orientation::X_UP:
				return { -v.y, v.x, v.z, v.w };
			}
			return v;
		}

		inline auto computeTangents(ofbx::Vec3* out, int32_t vertexCount, const ofbx::Vec3* vertices, const ofbx::Vec3* normals, const ofbx::Vec2* uvs)
		{
			for (int i = 0; i < vertexCount; i += 3)
			{
				const auto& v0 = vertices[i + 0];
				const auto& v1 = vertices[i + 1];
				const auto& v2 = vertices[i + 2];
				const auto& uv0 = uvs[i + 0];
				const auto& uv1 = uvs[i + 1];
				const auto& uv2 = uvs[i + 2];

				const ofbx::Vec3 dv10 = v1 - v0;
				const ofbx::Vec3 dv20 = v2 - v0;
				const ofbx::Vec2 duv10 = uv1 - uv0;
				const ofbx::Vec2 duv20 = uv2 - uv0;

				const float dir = duv20.x * duv10.y - duv20.y * duv10.x < 0 ? -1.f : 1.f;
				ofbx::Vec3 tangent;
				tangent.x = (dv20.x * duv10.y - dv10.x * duv20.y) * dir;
				tangent.y = (dv20.y * duv10.y - dv10.y * duv20.y) * dir;
				tangent.z = (dv20.z * duv10.y - dv10.z * duv20.y) * dir;
				const float l = 1 / sqrtf(float(tangent.x * tangent.x + tangent.y * tangent.y + tangent.z * tangent.z));
				tangent.x *= l;
				tangent.y *= l;
				tangent.z *= l;
				out[i + 0] = tangent;
				out[i + 1] = tangent;
				out[i + 2] = tangent;
			}
		}

		inline auto toMatrix(const ofbx::Matrix& mat)
		{
			glm::mat4  result;
			for (int32_t i = 0; i < 4; i++)
				for (int32_t j = 0; j < 4; j++)
					result[i][j] = (float)mat.m[i * 4 + j];
			return result;
		}

		inline auto getTransform(const ofbx::Object* object, Orientation orientation) -> component::Transform
		{
			component::Transform transform;
			ofbx::Vec3 p = object->getLocalTranslation();
			transform.setLocalPosition(fixOrientation({ static_cast<float>(p.x), static_cast<float>(p.y), static_cast<float>(p.z) }, orientation));
			ofbx::Vec3 r = object->getLocalRotation();
			transform.setLocalOrientation(fixOrientation(glm::vec3(static_cast<float>(r.x), static_cast<float>(r.y), static_cast<float>(r.z)), orientation));
			ofbx::Vec3 s = object->getLocalScaling();
			transform.setLocalScale({ static_cast<float>(s.x), static_cast<float>(s.y), static_cast<float>(s.z) });

			if (object->getParent())
			{
				transform.setWorldMatrix(getTransform(object->getParent(), orientation).getWorldMatrix());
			}
			else
			{
				transform.setWorldMatrix(glm::mat4(1));
			}
			return transform;
		}

		inline auto loadTexture(const ofbx::Material* material, ofbx::Texture::TextureType type) -> std::shared_ptr<Texture2D>
		{
			const ofbx::Texture* ofbxTexture = material->getTexture(type);
			std::shared_ptr<Texture2D> texture2D;
			if (ofbxTexture)
			{
				ofbx::DataView filename = ofbxTexture->getRelativeFileName();
				if (filename == "")
					filename = ofbxTexture->getFileName();

				char filePath[256];
				filename.toString(filePath);

				if (File::fileExists(filePath))
				{
					texture2D = Texture2D::create(filePath, filePath);
				}
				else
				{
					PLOGW("file {0} did not find", filePath);
				}
			}

			return texture2D;
		}

		inline auto loadMaterial(const ofbx::Material* material, bool animated)
		{
			auto pbrMaterial = std::make_shared<Material>();

			PBRMataterialTextures textures;
			MaterialProperties properties;

			properties.albedoColor = toGlm(material->getDiffuseColor());
			properties.metallicColor = toGlm(material->getSpecularColor());

			float roughness = 1.0f - std::sqrt(float(material->getShininess()) / 100.0f);
			properties.roughnessColor = glm::vec4(roughness);

			textures.albedo = loadTexture(material, ofbx::Texture::TextureType::DIFFUSE);
			textures.normal = loadTexture(material, ofbx::Texture::TextureType::NORMAL);
			textures.metallic = loadTexture(material, ofbx::Texture::TextureType::SPECULAR);
			textures.roughness = loadTexture(material, ofbx::Texture::TextureType::SHININESS);
			textures.emissive = loadTexture(material, ofbx::Texture::TextureType::EMISSIVE);
			textures.ao = loadTexture(material, ofbx::Texture::TextureType::AMBIENT);

			if (!textures.albedo)
				properties.usingAlbedoMap = 0.0f;
			if (!textures.normal)
				properties.usingNormalMap = 0.0f;
			if (!textures.metallic)
				properties.usingMetallicMap = 0.0f;
			if (!textures.roughness)
				properties.usingRoughnessMap = 0.0f;
			if (!textures.emissive)
				properties.usingEmissiveMap = 0.0f;
			if (!textures.ao)
				properties.usingAOMap = 0.0f;

			pbrMaterial->setTextures(textures);
			pbrMaterial->setMaterialProperites(properties);

			return pbrMaterial;
		}

		inline auto getOffsetMatrix(const ofbx::Mesh* mesh, const ofbx::Object* node) -> glm::mat4
		{
			auto* skin = mesh ? mesh->getGeometry()->getSkin() : nullptr;
			if (skin)
			{
				for (int i = 0, c = skin->getClusterCount(); i < c; i++)
				{
					const ofbx::Cluster* cluster = skin->getCluster(i);
					if (cluster->getLink() == node)
					{
						return toMatrix(cluster->getTransformLinkMatrix());
					}
				}
			}
			return toMatrix(node->getGlobalTransform());
		}

		inline auto loadBones(const ofbx::Object* parent, Skeleton* skeleton, int32_t parentId) -> void
		{
			int32_t i = 0;
			while (const ofbx::Object* child = parent->resolveObjectLink(i++))
			{
				if (child->getType() == ofbx::Object::Type::LIMB_NODE)
				{
					auto boneIndex = skeleton->getBoneIndex(child->name);//create a bone if missing
					if (boneIndex == -1)
					{
						auto& newBone = skeleton->createBone();
						newBone.name = child->name;
						newBone.offsetMatrix = toMatrix(child->getLocalTransform());
						boneIndex = newBone.id;
						newBone.parentIdx = parentId;
						auto& parent = skeleton->getBones()[parentId];
						parent.children.push_back(boneIndex);
						loadBones(child, skeleton, boneIndex);
					}
				}
			}
		}

		inline auto print(Skeleton* skeleton, const Bone& bone, int32_t level) -> void
		{
			std::string str;
			for (auto i = 0; i < level; i++)
			{
				str += "--";
			}
			PLOGW("{0} ,{1}", str, bone.name);
			for (auto child : bone.children)
			{
				print(skeleton, skeleton->getBones()[child], level + 1);
			}
		}
	};
	auto FBXLoader::load(const std::string& obj, const std::string& extension, std::unordered_map<std::string, std::shared_ptr<Mesh>>& meshes, std::shared_ptr<Skeleton>& skeleton)-> void
	{
		mio::mmap_source mmap(obj);
		PENGINE_ASSERT(mmap.is_open(), "open fbx file failed");

		constexpr bool ignoreGeometry = false;
		const uint64_t flags = ignoreGeometry ? (uint64_t)ofbx::LoadFlags::IGNORE_GEOMETRY : (uint64_t)ofbx::LoadFlags::TRIANGULATE;
		auto scene = ofbx::load((uint8_t*)mmap.data(), mmap.size(), flags);

		const ofbx::GlobalSettings* settings = scene->getGlobalSettings();

		Orientation orientation = Orientation::Y_UP;

		switch (settings->UpAxis)
		{
		case ofbx::UpVector_AxisX:
			orientation = Orientation::X_UP;
			break;
		case ofbx::UpVector_AxisY:
			orientation = Orientation::Y_UP;
			break;
		case ofbx::UpVector_AxisZ:
			orientation = Orientation::Z_UP;
			break;
		}

		int32_t meshCount = scene->getMeshCount();

		for (int32_t i = 0; i < meshCount; ++i)
		{
			const auto fbxMesh = (const ofbx::Mesh*)scene->getMesh(i);
			const auto geom = fbxMesh->getGeometry();
			const auto numIndices = geom->getIndexCount();
			const auto vertexCount = geom->getVertexCount();
			const auto vertices = geom->getVertices();
			const auto normals = geom->getNormals();
			const auto tangents = geom->getTangents();
			const auto colors = geom->getColors();
			const auto uvs = geom->getUVs();

			std::vector<Vertex> tempVertices;
			tempVertices.resize(vertexCount);
			std::vector<uint32_t> indicesArray;
			indicesArray.resize(numIndices);
			auto boundingBox = std::make_shared<BoundingBox>();

			const auto indices = geom->getFaceIndices();

			ofbx::Vec3* generatedTangents = nullptr;
			if (!tangents && normals && uvs)
			{
				generatedTangents = new ofbx::Vec3[vertexCount];
				computeTangents(generatedTangents, vertexCount, vertices, normals, uvs);
			}

			auto transform = getTransform(fbxMesh, orientation);

			for (int32_t i = 0; i < vertexCount; ++i)
			{
				const ofbx::Vec3& cp = vertices[i];
				auto& vertex = tempVertices[i];
				vertex.pos = transform.getWorldMatrix() * glm::vec4(float(cp.x), float(cp.y), float(cp.z), 1.0);
				fixOrientation(vertex.pos, orientation);
				boundingBox->merge(vertex.pos);

				if (normals)
				{
					glm::mat3 matrix(transform.getWorldMatrix());
					vertex.normal = glm::transpose(glm::inverse(matrix)) * glm::normalize(glm::vec3{ float(normals[i].x), float(normals[i].y), float(normals[i].z) });
				}
				if (uvs)
					vertex.texCoord = { float(uvs[i].x), 1.0f - float(uvs[i].y) };
				if (colors)
					vertex.color = { float(colors[i].x), float(colors[i].y), float(colors[i].z), float(colors[i].w) };
				else
					vertex.color = { 1,1,1,1 };
				if (tangents)
					vertex.tangent = transform.getWorldMatrix() * glm::vec4(float(tangents[i].x), float(tangents[i].y), float(tangents[i].z), 1.0);

				fixOrientation(vertex.normal, orientation);
				fixOrientation(vertex.tangent, orientation);
			}

			for (int32_t i = 0; i < numIndices; i++)
			{
				int32_t index = (i % 3 == 2) ? (-indices[i] - 1) : indices[i];
				indicesArray[i] = index;
			}

			const ofbx::Material* material = fbxMesh->getMaterialCount() > 0 ? fbxMesh->getMaterial(0) : nullptr;

			auto mesh = std::make_shared<Mesh>(indicesArray, tempVertices);
			auto pbrMaterial = std::make_shared<Material>();
			if (material)
			{
				auto pbrMaterial = loadMaterial(material, false);
				mesh->setMaterial(pbrMaterial);
			}

			std::string name = fbxMesh->name;

			PENGINE_ASSERT(name != "", "name should not be null");

			mesh->setName(name);
			meshes.emplace(name, mesh);

			if (generatedTangents)
				delete[] generatedTangents;

		}

		auto skeletonPtr = new Skeleton();

		int32_t i = 0;
		while (const ofbx::Object* bone = scene->getRoot()->resolveObjectLink(i++))
		{
			if (bone->getType() == ofbx::Object::Type::LIMB_NODE)
			{
				auto boneIndex = skeletonPtr->getBoneIndex(bone->name);//create a bone if missing
				if (boneIndex == -1)
				{
					auto& newBone = skeletonPtr->createBone();
					newBone.name = bone->name;
					newBone.offsetMatrix = toMatrix(bone->getLocalTransform());
					boneIndex = newBone.id;
				}
				loadBones(bone, skeletonPtr, boneIndex);
			}
		}

		if (!skeletonPtr->getBones().empty())
		{
			skeleton.reset(skeletonPtr);
		}
		else
		{
			delete skeletonPtr;
		}
	}
	auto FBXLoader::release() -> void 
	{
		
	}
};