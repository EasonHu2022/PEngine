#pragma once
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/glm.hpp"
#include "glm/ext.hpp"
#include "glm/gtx/hash.hpp"

namespace pengine
{
	struct Vertex
	{
		glm::vec3 pos;
		glm::vec4 color;
		glm::vec2 texCoord;
		glm::vec3 normal;
		glm::vec3 tangent;
		inline auto operator-(const Vertex& right) -> Vertex
		{
			Vertex ret;
			ret.pos = pos - right.pos;
			ret.color = color - right.color;
			ret.texCoord = texCoord - right.texCoord;
			ret.normal = normal - right.normal;
			return ret;
		}

		inline auto operator+(const Vertex& right) -> Vertex
		{
			Vertex ret;
			ret.pos = pos + right.pos;
			ret.color = color + right.color;
			ret.texCoord = texCoord + right.texCoord;
			ret.normal = normal + right.normal;
			return ret;
		}

		inline auto operator*(float factor) -> Vertex
		{
			Vertex ret;
			ret.pos = pos * factor;
			ret.color = color * factor;
			ret.texCoord = texCoord * factor;
			ret.normal = normal * factor;
			return ret;
		}

		inline auto operator==(const Vertex& other) const -> bool
		{
			return pos == other.pos && color == other.color && texCoord == other.texCoord && normal == other.normal && tangent == other.tangent;
		}
	};
};

namespace std {
	//hash for vertex
	template<> struct hash<pengine::Vertex> {
		size_t operator()(const pengine::Vertex& vertex) const {
			return
				((hash<glm::vec3>()(vertex.pos) ^
					(hash<glm::vec2>()(vertex.texCoord) << 1) ^
					(hash<glm::vec4>()(vertex.color) << 1) ^
					(hash<glm::vec3>()(vertex.normal) << 1)
					));
		}
	};
}
