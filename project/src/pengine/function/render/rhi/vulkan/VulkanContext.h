#pragma once
#include "function/render/rhi/GraphicsContext.h"
#include "core/core.h"
#include "VKHelper.h"
#include <deque>
#include <functional>
namespace pengine
{
	class VulkanContext : public GraphicsContext
	{
	public:
		VulkanContext();
		~VulkanContext();
		NO_COPYABLE(VulkanContext);
		void init() override;

		void present() override;

		void waitIdle() const override;
		auto getMinUniformBufferOffsetAlignment() const->size_t override;
		void onImGui() override;
		static auto get()->std::shared_ptr<VulkanContext>;
		auto setupDebug() -> void;
		inline const auto getVkInstance() const
		{
			return vkInstance;
		}
		inline auto getVkInstance()
		{
			return vkInstance;
		}
		inline float getGPUMemoryUsed()  override
		{
			return 0;
		}

		inline float getTotalGPUMemory()  override
		{
			return 0;
		}
		struct CommandQueue
		{
			CommandQueue() = default;
			CommandQueue(const CommandQueue&) = delete;
			auto operator=(const CommandQueue&)->CommandQueue & = delete;

			std::deque<std::function<void()>> deletors;

			template <typename F>
			inline auto emplace(F&& function)
			{
				deletors.emplace_back(function);
			}

			inline auto flush()
			{
				for (auto it = deletors.rbegin(); it != deletors.rend(); it++)
				{
					(*it)();
				}
				deletors.clear();
			}
		};
		static auto getDeletionQueue()->CommandQueue&;
		static auto getDeletionQueue(uint32_t index)->CommandQueue&;

	private:
		//bind to triple buffer
		CommandQueue deletionQueue[3];

		VkInstance vkInstance = VK_NULL_HANDLE;
		std::vector<const char*>          instanceLayerNames;
		std::vector<const char*>          instanceExtensionNames;
		std::vector<VkLayerProperties>     instanceLayers;
		std::vector<VkExtensionProperties> instanceExtensions;
		VkDebugUtilsMessengerEXT debugMessenger = VK_NULL_HANDLE;
		void createInstance();
	};
}