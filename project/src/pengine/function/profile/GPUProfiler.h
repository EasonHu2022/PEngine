#pragma once
#ifdef PEGNINE_WINDOWS
#define TRACY_CALLSTACK 1
#endif // PEGNINE_WINDOWS
#ifdef PENGINE_VULKAN
#include "function/render/rhi/vulkan/VKHelper.h"
#include "tracy/TracyVulkan.hpp"
#endif // PENGINE_VULKAN
#define GPU_PROFILE(ctx,cb,name)  TracyVkZone(ctx,cb,name)
#define GPU_PROFILEC(ctx,cb,name,col)  TracyVkZoneC(ctx,cb,name,col)
#define GPU_EVENTCOLLECT(ctx,cb)  TracyVkCollect(ctx,cb)

