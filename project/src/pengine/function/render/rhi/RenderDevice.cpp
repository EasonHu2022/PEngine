#include "RenderDevice.h"
#include "vulkan/VulkanRenderDevice.h"
#include "Application.h"
#include "function/engine/Mesh.h"
namespace pengine
{
	std::shared_ptr<RenderDevice> RenderDevice::create()
	{
#ifdef PENGINE_VULKAN
		return std::make_shared<VulkanRenderDevice>();
#endif        // PENGINE_VULKAN
#ifdef PENGINE_OPENGL
		//return std::make_shared<VulkanRenderDevice>();
#endif        // PENGINE_VULKAN
	}

	auto RenderDevice::drawMesh(CommandBuffer* cmdBuffer, Pipeline* pipeline, Mesh* mesh) -> void
	{
		mesh->getVertexBuffer()->bind(cmdBuffer, pipeline);
		mesh->getIndexBuffer()->bind(cmdBuffer);
		drawIndexed(cmdBuffer, DrawType::Triangle, mesh->getIndexBuffer()->getCount());
		mesh->getVertexBuffer()->unbind();
		mesh->getIndexBuffer()->unbind();
	}

	auto RenderDevice::present() -> void
	{
		Application::getRenderDevice()->presentInternal();
	}

	auto RenderDevice::present(CommandBuffer* commandBuffer) -> void
	{
		Application::getRenderDevice()->presentInternal(commandBuffer);
	}

	auto RenderDevice::bindDescriptorSets(Pipeline* pipeline, CommandBuffer* commandBuffer, uint32_t dynamicOffset, const std::vector<std::shared_ptr<DescriptorSet>>& descriptorSets) -> void
	{
		Application::getRenderDevice()->bindDescriptorSetsInternal(pipeline, commandBuffer, dynamicOffset, descriptorSets);
	}

	auto RenderDevice::drawIndexed(CommandBuffer* commandBuffer, DrawType type, uint32_t count, uint32_t start) -> void
	{
		Application::getRenderDevice()->drawIndexedInternal(commandBuffer, type, count, start);
	}

};