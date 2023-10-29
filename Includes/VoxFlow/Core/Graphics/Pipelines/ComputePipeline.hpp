// Author : snowapril

#ifndef VOXEL_FLOW_COMPUTE_PIPELINE_HPP
#define VOXEL_FLOW_COMPUTE_PIPELINE_HPP

#include <VoxFlow/Core/Graphics/Pipelines/BasePipeline.hpp>
#include <memory>
#include <string>

namespace VoxFlow
{
class ShaderModule;

class ComputePipeline : public BasePipeline
{
 public:
    explicit ComputePipeline(PipelineStreamingContext* pipelineStreamingContext, const ShaderPathInfo& shaderPath);
    ~ComputePipeline() override;
    ComputePipeline(ComputePipeline&& other) noexcept;
    ComputePipeline& operator=(ComputePipeline&& other) noexcept;

    [[nodiscard]] VkPipelineBindPoint getBindPoint() const noexcept override
    {
        return VK_PIPELINE_BIND_POINT_COMPUTE;
    }

 public:
    /**
     * Create compute pipeline with owned shader modules
     */
    bool initialize();
};
}  // namespace VoxFlow

#endif