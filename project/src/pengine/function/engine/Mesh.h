#pragma once
#include "core/core.h"
#include <vector>
#include "Vertex.h"
#include <string>
#include "function/render/rhi/IndexBuffer.h"
#include "function/render/rhi/VertexBuffer.h"
namespace pengine
{
	class DescriptorSet;
	class Material;

	class PENGINE_API Mesh 
	{
	public:
		Mesh() = default;
		Mesh(const std::vector<uint32_t>& indices, const std::vector<Vertex>& vertices);
		Mesh(const std::shared_ptr<VertexBuffer>& vertexBuffer,
			const std::shared_ptr<IndexBuffer>& indexBuffer);
		~Mesh();
		static void generateNormals(std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices);
		static void generateTangents(std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices);
		inline auto setName(const std::string& name)
		{
			this->name = name;
		}
		auto setIndicies(uint32_t range) -> void;
		inline auto setMaterial(const std::shared_ptr<Material>& material)
		{
			materials.clear();
			materials.emplace_back(material);
		}

		inline auto setMaterial(const std::vector<std::shared_ptr<Material>>& material)
		{
			materials = material;
		}

		inline auto setIndicesSize(uint32_t size)
		{
			this->size = size;
		}
		inline auto getSize()
		{
			return size;
		}

		inline auto setSubMeshCount(uint32_t subMesh)
		{
			subMeshCount = subMesh;
		}

		inline auto setSubMeshIndex(const std::vector<uint32_t>& idx)
		{
			subMeshIndex = idx;
		}

		inline auto getSubMeshCount() const { return subMeshCount; }

		inline auto& getSubMeshIndex() const { return subMeshIndex; }

		inline auto& getIndexBuffer()
		{
			return indexBuffer;
		}
		inline auto& getVertexBuffer()
		{
			return vertexBuffer;
		}
		inline auto& getMaterial()
		{
			return materials;
		}
		inline auto& getDescriptorSet()
		{
			return descriptorSet;
		}
		inline void setDescriptorSet(const std::shared_ptr<DescriptorSet>& set)
		{
			descriptorSet = set;
		}
		inline auto& isActive() const
		{
			return active;
		}
		inline auto setActive(bool active)
		{
			this->active = active;
		}
		static auto createQuad(bool screen = false) -> std::shared_ptr<Mesh>;
		static auto createQuaterScreenQuad(bool screen = false) -> std::shared_ptr<Mesh>;
		static auto createCube() -> std::shared_ptr<Mesh>;
		static auto createPyramid() -> std::shared_ptr<Mesh>;
		static auto createSphere(uint32_t xSegments = 64, uint32_t ySegments = 64) -> std::shared_ptr<Mesh>;
		static auto createPlane(float w, float h, const glm::vec3& normal) -> std::shared_ptr<Mesh>;

	protected:
		static glm::vec3 generateTangent(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c, const glm::vec2& ta, const glm::vec2& tb, const glm::vec2& tc);
		std::string name;
		uint32_t    size = 0;
		bool        active = true;
		std::shared_ptr<IndexBuffer>   indexBuffer;
		std::shared_ptr<VertexBuffer>  vertexBuffer;
		std::vector<std::shared_ptr<Material>> materials;
		std::shared_ptr<DescriptorSet> descriptorSet;
		uint32_t subMeshCount = 0;
		std::vector<uint32_t> subMeshIndex;
	};
}

