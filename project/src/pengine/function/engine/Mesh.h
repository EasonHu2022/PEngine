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

		static auto createQuad(bool screen = false) -> std::shared_ptr<Mesh>;
		static auto createQuaterScreenQuad(bool screen = false) -> std::shared_ptr<Mesh>;
		static auto createCube() -> std::shared_ptr<Mesh>;
		static auto createPyramid() -> std::shared_ptr<Mesh>;
		static auto createSphere(uint32_t xSegments = 64, uint32_t ySegments = 64) -> std::shared_ptr<Mesh>;
		static auto createPlane(float w, float h, const glm::vec3& normal) -> std::shared_ptr<Mesh>;

	protected:
		static glm::vec3 generateTangent(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c, const glm::vec2& ta, const glm::vec2& tb, const glm::vec2& tc);
		std::string name;
		std::shared_ptr<IndexBuffer>   indexBuffer;
		std::shared_ptr<VertexBuffer>  vertexBuffer;
		std::vector<std::shared_ptr<Material>> materials;
		uint32_t subMeshCount = 0;
		std::vector<uint32_t> subMeshIndex;
	};
}

