#include "RenderDevice.h"
#include "vulkan/VulkanRenderDevice.h"
#include "Application.h"
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

	auto RenderDevice::present() -> void
	{
		Application::getRenderDevice()->presentInternal();
	}

	auto RenderDevice::present(CommandBuffer* commandBuffer) -> void
	{
		Application::getRenderDevice()->presentInternal(commandBuffer);
	}

};