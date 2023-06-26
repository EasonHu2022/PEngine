#include "MeshRenderer.h"
namespace pengine
{
	namespace component
	{
		MeshRenderer::MeshRenderer(const std::shared_ptr<pengine::Mesh>& mesh)
		{
		}
		auto MeshRenderer::isActive() const -> bool
		{
			return false;
		}
		auto MeshRenderer::getMesh(const std::string& name) -> void
		{
		}
	};
};