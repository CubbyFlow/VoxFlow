// Author : snowapril

#ifndef VOXEL_FLOW_FRAME_GRAPH_PASS_IMPL_HPP
#define VOXEL_FLOW_FRAME_GRAPH_PASS_IMPL_HPP

#include <VoxFlow/Core/FrameGraph/FrameGraphPass.hpp>

namespace VoxFlow
{
namespace FrameGraph
{
template <typename PassDataType, typename ExecutePhase>
FrameGraphPass<PassDataType, ExecutePhase>::FrameGraphPass(
    ExecutePhase&& executePhase)
    : _executionPhaseLambda(std::move(executePhase))
{
}
template <typename PassDataType, typename ExecutePhase>
FrameGraphPass<PassDataType, ExecutePhase>::~FrameGraphPass()
{
}
}  // namespace FrameGraph

}  // namespace VoxFlow

#endif