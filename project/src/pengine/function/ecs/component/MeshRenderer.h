#pragma once
#include "Component.h"
#include "function/engine/Mesh.h"
#include "function/resource/ModelResource.h"
namespace pengine
{
	namespace component
	{
		enum class PrimitiveType : int32_t
		{
			Plane = 0,
			Quad = 1,
			Cube = 2,
			Pyramid = 3,
			Sphere = 4,
			Capsule = 5,
			Cylinder = 6,
			Terrain = 7,
			File = 8,
			Length
		};
		class PENGINE_API Model : public Component
		{
		public:
			Model() = default;
			Model(const std::string& file);
			std::string                   filePath;
			PrimitiveType                 type = PrimitiveType::Length;
			std::shared_ptr<ModelResource> resource;
		};

		class PENGINE_API SkinnedMeshRenderer : public Component
		{
		public:
			SkinnedMeshRenderer(const std::shared_ptr<Mesh>& mesh);
			SkinnedMeshRenderer() = default;
			inline auto& getMesh()
			{
				if (mesh == nullptr)
					getMesh(meshName);
				return mesh;
			}
			auto isActive() const -> bool;
			inline auto isCastShadow() const { return castShadow; }

			inline auto setCastShadow(bool shadow) { castShadow = shadow; }

			bool castShadow = true;

		private:
			std::shared_ptr<Mesh>     mesh;
			auto                      getMesh(const std::string& name) -> void;
			std::string               meshName;
		};

		class PENGINE_API MeshRenderer : public Component
		{
		public:
			MeshRenderer() = default;
			MeshRenderer(const std::shared_ptr<pengine::Mesh>& mesh);

			inline auto& getMesh()
			{
				if (mesh == nullptr)
					getMesh(meshName);
				return mesh;
			}

			auto isActive() const -> bool;

			inline auto isCastShadow() const { return m_bCastShadow; }

			inline auto setCastShadow(bool shadow) { m_bCastShadow = shadow; }

			bool m_bCastShadow = true;

		private:
			std::shared_ptr<pengine::Mesh>     mesh;
			auto                      getMesh(const std::string& name) -> void;
			std::string               meshName;
		};
	}
}