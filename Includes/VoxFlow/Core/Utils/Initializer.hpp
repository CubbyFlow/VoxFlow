// Author : snowapril

#ifndef VOXEL_FLOW_INITIALIZER_HPP
#define VOXEL_FLOW_INITIALIZER_HPP

#include <VoxFlow/Core/Devices/Context.hpp>
#include <VoxFlow/Core/Utils/pch.hpp>
#include <string>
#include <vector>

namespace VoxFlow
{
class Initializer
{
 public:
    Initializer() = delete;

    template <typename InfoType>
    [[nodiscard]] static InfoType MakeInfo();
};
}  // namespace VoxFlow

#include <VoxFlow/Core/Utils/Initializer-Impl.hpp>

#endif