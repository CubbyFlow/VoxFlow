// Author : snowapril

#ifndef VOXEL_FLOW_COMPUTE_PIPELINE_HPP
#define VOXEL_FLOW_COMPUTE_PIPELINE_HPP

#include <VoxFlow/Core/Graphics/Pipelines/BasePipeline.hpp>

namespace VoxFlow
{
class LogicalDevice;

class ComputePipeline : BasePipeline
{
 public:
    explicit ComputePipeline(const std::shared_ptr<LogicalDevice>& device);
    ~ComputePipeline() override;
    ComputePipeline(ComputePipeline&& other) noexcept;
    ComputePipeline& operator=(ComputePipeline&& other) noexcept;
};
}  // namespace VoxFlow

#endif