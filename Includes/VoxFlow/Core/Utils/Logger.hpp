// Author : snowapril

#ifndef VOXEL_FLOW_LOGGER_HPP
#define VOXEL_FLOW_LOGGER_HPP

#include <cstdlib>
#include <spdlog/spdlog.h>

namespace VoxFlow
{
#define VK_ASSERT(x)                                                     \
	do                                                                   \
	{                                                                    \
        if (!static_cast<bool>(x))                                       \
        {                                                                \
            spdlog::error("Vulkan Error at {}:{}.", __FILE__, __LINE__); \
            std::abort();                                                \
        }                                                                \
	} while(false);
}  // namespace VoxFlow

#endif