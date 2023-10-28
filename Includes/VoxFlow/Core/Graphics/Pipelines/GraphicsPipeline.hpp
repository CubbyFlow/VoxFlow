// Author : snowapril

#ifndef VOXEL_FLOW_GRAPHICS_PIPELINE_HPP
#define VOXEL_FLOW_GRAPHICS_PIPELINE_HPP

#include <VoxFlow/Core/Graphics/Pipelines/BasePipeline.hpp>
#include <VoxFlow/Core/Graphics/Pipelines/PipelineStateObject.hpp>
#include <memory>
#include <initializer_list>
#include <vector>

namespace VoxFlow
{
class LogicalDevice;
class ShaderModule;
class RenderPass;

class GraphicsPipeline : public BasePipeline
{
 public:
    explicit GraphicsPipeline(LogicalDevice* logicalDevice,
                              std::initializer_list<const char*>&& shaderPaths);
    ~GraphicsPipeline() override;
    GraphicsPipeline(GraphicsPipeline&& other) noexcept;
    GraphicsPipeline& operator=(GraphicsPipeline&& other) noexcept;

    [[nodiscard]] VkPipelineBindPoint getBindPoint() const noexcept override
    {
        return VK_PIPELINE_BIND_POINT_GRAPHICS;
    }

    inline void setPipelineState(const GraphicsPipelineState& pipelineState)
    {
        _pipelineState = pipelineState;
    }

    [[nodiscard]] inline GraphicsPipelineState& getPipelineState()
    {
        return _pipelineState;
    }

    [[nodiscard]] inline const GraphicsPipelineState& getPipelineState() const
    {
        return _pipelineState;
    }

public:
    /**
    * Create graphics pipeline with given renderpass and owned shader modules
    */
   bool initialize(RenderPass* renderPass);

private:
   GraphicsPipelineState _pipelineState;
};
}  // namespace VoxFlow

#endif