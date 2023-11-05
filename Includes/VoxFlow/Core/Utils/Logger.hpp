// Author : snowapril

#ifndef VOXEL_FLOW_LOGGER_HPP
#define VOXEL_FLOW_LOGGER_HPP

#include <spdlog/spdlog.h>
#include <VoxFlow/Core/Utils/DebugUtil.hpp>
#include <cstdlib>

namespace VoxFlow
{
// #ifdef VOXFLOW_DEBUG
#define VK_ASSERT(returnCode)                                                                                \
    do                                                                                                       \
    {                                                                                                        \
        if (!static_cast<bool>(returnCode == VK_SUCCESS))                                                    \
        {                                                                                                    \
            spdlog::error("Vulkan Error ({}) at {}:{}.", getVkResultString(returnCode), __FILE__, __LINE__); \
            if (returnCode == VK_ERROR_DEVICE_LOST)                                                          \
                DeviceRemoveTracker::get()->onDeviceRemoved();                                               \
            std::abort();                                                                                    \
        }                                                                                                    \
    } while (false)

#define VOX_ASSERT(expr, msg, ...)                                           \
    do                                                                       \
    {                                                                        \
        if (!static_cast<bool>(expr))                                        \
        {                                                                    \
            spdlog::error("{} at {}:{}.", fmt, args..., __FILE__, __LINE__); \
            DebugUtil::DebugBreak();                                         \
        }                                                                    \
    } while (false);
#define VOX_ASSERT_RETURN_VOID(expr, msg, ...)                               \
    do                                                                       \
    {                                                                        \
        if (!static_cast<bool>(expr))                                        \
        {                                                                    \
            spdlog::error("{} at {}:{}.", fmt, args..., __FILE__, __LINE__); \
            DebugUtil::DebugBreak();                                         \
            return;                                                          \
        }                                                                    \
    } while (false);
#define VOX_ASSERT_RETURN_BOOL(expr, msg, ...)                               \
    do                                                                       \
    {                                                                        \
        if (!static_cast<bool>(expr))                                        \
        {                                                                    \
            spdlog::error("{} at {}:{}.", fmt, args..., __FILE__, __LINE__); \
            DebugUtil::DebugBreak();                                         \
            return false;                                                    \
        }                                                                    \
    } while (false);
#define VOX_ASSERT_RETURN_NULL(expr, msg, ...)                               \
    do                                                                       \
    {                                                                        \
        if (!static_cast<bool>(expr))                                        \
        {                                                                    \
            spdlog::error("{} at {}:{}.", fmt, args..., __FILE__, __LINE__); \
            DebugUtil::DebugBreak();                                         \
            return nullptr;                                                  \
        }                                                                    \
    } while (false);
// #else
// #define VK_ASSERT(x)
// #define VOX_ASSERT(x, msg, ...)
// #define VOX_ASSERT_RETURN_VOID(expr, msg, ...)
// #define VOX_ASSERT_RETURN_BOOL(expr, msg, ...)
// #define VOX_ASSERT_RETURN_NULL(expr, msg, ...)
// #endif

}  // namespace VoxFlow

#endif