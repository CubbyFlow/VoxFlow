// Author : snowapril

#ifndef VOXEL_FLOW_FRAME_GRAPH_CONCEPT_HPP
#define VOXEL_FLOW_FRAME_GRAPH_CONCEPT_HPP

#include <concepts>
#include <string>
#include <type_traits>

namespace VoxFlow
{

class RenderResourceAllocator;

namespace RenderGraph
{
template <typename Type>
concept ResourceConcept = requires(Type resource)
{
    typename Type::Descriptor;
    typename Type::Usage;

    requires std::is_default_constructible_v<Type> and std::is_move_constructible_v<Type>;

    {
        resource.create((RenderResourceAllocator *)nullptr, std::string{}, typename Type::Descriptor{}, typename Type::Usage{})
        } -> std::same_as<bool>;
    {
        resource.destroy((RenderResourceAllocator *)nullptr)
        } -> std::same_as<void>;
};

template <typename Type>
concept RenderPassConcept = requires(Type resource)
{
    typename Type::Descriptor;
    requires std::is_default_constructible_v<Type> and std::is_move_constructible_v<Type>;
};
}  // namespace RenderGraph

}  // namespace VoxFlow

#endif