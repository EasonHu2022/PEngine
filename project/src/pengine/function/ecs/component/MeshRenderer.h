#pragma once
#include "Component.h"
#include "function/engine/Mesh.h"
namespace pengine
{
	namespace component
	{
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