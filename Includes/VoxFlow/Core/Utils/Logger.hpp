// Author : snowapril

#ifndef VOXEL_FLOW_LOGGER_HPP
#define VOXEL_FLOW_LOGGER_HPP

#include <spdlog/spdlog.h>
#include <VoxFlow/Core/Utils/DebugUtil.hpp>
#include <cstdlib>

namespace VoxFlow
{
// #ifdef VOXFLOW_DEBUG
#define VK_ASSERT(returnCode)                                                 \
    do                                                                        \
    {                                                                         \
        if (!static_cast<bool>(returnCode == VK_SUCCESS))                     \
        {                                                                     \
            spdlog::error("Vulkan Error ({}) at {}:{}.",                      \
                          getVkResultString(returnCode), __FILE__, __LINE__); \
            if (returnCode == VK_ERROR_DEVICE_LOST)                           \
                DeviceRemoveTracker::get()->onDeviceRemoved();                \
            std::abort();                                                     \
        }                                                                     \
    } while (false)

template <typename... Args>
void voxAssertImpl(bool expr, const char* fmt, Args... args)
{
    do
    {
        if (!static_cast<bool>(expr))
        {
            spdlog::error("%s at {}:{}.", fmt, args..., __FILE__, __LINE__);
            DebugUtil::DebugBreak();
        }
    } while (false);
}

#define VOX_ASSERT(expr, msg, ...) voxAssertImpl(expr, msg, ##__VA_ARGS__)
// #else
// #define VK_ASSERT(x)
// #define VOX_ASSERT(x, msg, ...)
// #endif

}  // namespace VoxFlow

#endif