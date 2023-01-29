// Author : snowapril

#ifndef VOXEL_FLOW_LOGGER_HPP
#define VOXEL_FLOW_LOGGER_HPP

#include <spdlog/spdlog.h>
#include <VoxFlow/Core/Utils/DebugUtil.hpp>
#include <cstdlib>

namespace VoxFlow
{
#ifdef VOXFLOW_DEBUG
#define VK_ASSERT(return_code)                                                 \
    do                                                                         \
    {                                                                          \
        if (!static_cast<bool>(return_code == VK_SUCCESS))                     \
        {                                                                      \
            spdlog::error("Vulkan Error ({}) at {}:{}.",                       \
                          getVkResultString(return_code), __FILE__, __LINE__); \
            std::abort();                                                      \
        }                                                                      \
    } while (false)

template <typename... Args>
void voxAssertImpl(bool expr, const char* fmt, Args... args)
{
    do
    {
        if (!static_cast<bool>(expr))
        {
            spdlog::error("%s at {}:{}.", fmt, args..., __FILE__, __LINE__);
            std::abort();
        }
    } while (false);
}

#define VOX_ASSERT(expr, msg, ...) voxAssertImpl(expr, msg, ##__VA_ARGS__)
#else
#define VK_ASSERT(x)
#define VOX_ASSERT(x, msg, ...)
#endif

}  // namespace VoxFlow

#endif