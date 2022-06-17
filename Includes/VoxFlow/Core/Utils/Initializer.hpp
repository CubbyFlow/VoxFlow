// Author : snowapril

#ifndef VOXEL_FLOW_INITIALIZER_HPP
#define VOXEL_FLOW_INITIALIZER_HPP

namespace VoxFlow
{
class Initializer
{
 public:
    Initializer() = delete;

    template <typename InfoType>
    [[nodiscard]] static InfoType MakeInfo() noexcept;
};
}  // namespace VoxFlow

#include <VoxFlow/Core/Utils/Initializer-Impl.hpp>

#endif