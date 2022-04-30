// Author : snowapril

#ifndef VOXEL_FLOW_LOGGER_HPP
#define VOXEL_FLOW_LOGGER_HPP

#include <spdlog/spdlog.h>
#include <cstdlib>

namespace VoxFlow
{
#ifdef VOXFLOW_DEBUG
#define VK_ASSERT(x)                                                     \
    do                                                                   \
    {                                                                    \
        if (!static_cast<bool>(x))                                       \
        {                                                                \
            spdlog::error("Vulkan Error at {}:{}.", __FILE__, __LINE__); \
            std::abort();                                                \
        }                                                                \
    } while (false)
#else
#define VK_ASSERT(x) ((void)0)
#endif

#ifdef VOXFLOW_DEBUG
#define ABORT_WITH_MSG(msg)                                        \
    {                                                              \
        spdlog::error("\"{}\" at {}:{}", msg, __FILE__, __LINE__); \
    }
#else
#define ABORT_WITH_MSG(msg) ((void)msg)
#endif
}  // namespace VoxFlow

#endif