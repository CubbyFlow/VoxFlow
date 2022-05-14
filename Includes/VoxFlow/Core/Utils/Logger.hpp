// Author : snowapril

#ifndef VOXEL_FLOW_LOGGER_HPP
#define VOXEL_FLOW_LOGGER_HPP

#include <spdlog/spdlog.h>
#include <cstdlib>

namespace VoxFlow
{
#ifdef VOXFLOW_DEBUG
#define VK_ASSERT(return_code)                                        \
    do                                                                \
    {                                                                 \
        if (!static_cast<bool>(return_code == VK_SUCCESS))            \
        {                                                             \
            spdlog::error("Vulkan Error ({}) at {}:{}.", return_code, \
                          __FILE__, __LINE__);                        \
            std::abort();                                             \
        }                                                             \
    } while (false)
#else
#define VK_ASSERT(x) ((void)0)
#endif



}  // namespace VoxFlow

#endif