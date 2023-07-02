// Author : snowapril

#ifndef VOXEL_FLOW_FRAME_GRAPH_CONCEPT_HPP
#define VOXEL_FLOW_FRAME_GRAPH_CONCEPT_HPP

#include <concepts>
#include <type_traits>

namespace VoxFlow
{

namespace FrameGraph
{
template <typename Type>
concept ResourceConcept = requires(Type resource)
{
    typename Type::Descriptor;
    requires std::is_default_constructible_v<Type> and
        std::is_move_constructible_v<Type>;
};

template <typename Type>
concept RenderPassConcept = requires(Type resource)
{
    typename Type::Descriptor;
    requires std::is_default_constructible_v<Type> and
        std::is_move_constructible_v<Type>;
};
}  // namespace FrameGraph

}  // namespace VoxFlow

#endif