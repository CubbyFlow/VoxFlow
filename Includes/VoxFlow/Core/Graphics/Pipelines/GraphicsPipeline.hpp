// Author : snowapril

#ifndef VOXEL_FLOW_GRAPHICS_PIPELINE_HPP
#define VOXEL_FLOW_GRAPHICS_PIPELINE_HPP

#include <VoxFlow/Core/Graphics/Pipelines/BasePipeline.hpp>
#include <memory>

namespace VoxFlow
{
class LogicalDevice;
class ShaderModule;
class RenderPass;
class GraphicsPipeline : public BasePipeline
{
 public:
    explicit GraphicsPipeline(
        LogicalDevice* logicalDevice,
        std::vector<std::shared_ptr<ShaderModule>>&& shaderModules,
        const std::shared_ptr<PipelineLayout>& layout);
    ~GraphicsPipeline() override;
    GraphicsPipeline(GraphicsPipeline&& other) noexcept;
    GraphicsPipeline& operator=(GraphicsPipeline&& other) noexcept;

    [[nodiscard]] VkPipelineBindPoint getBindPoint() const noexcept override
    {
        return VK_PIPELINE_BIND_POINT_GRAPHICS;
    }

public:
    bool initialize(const std::shared_ptr<RenderPass>& renderPass);
};
}  // namespace VoxFlow

#endif