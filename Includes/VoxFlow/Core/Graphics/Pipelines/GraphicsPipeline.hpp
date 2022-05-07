// Author : snowapril

#ifndef VOXEL_FLOW_GRAPHICS_PIPELINE_HPP
#define VOXEL_FLOW_GRAPHICS_PIPELINE_HPP

#include <VoxFlow/Core/Graphics/Pipelines/BasePipeline.hpp>

namespace VoxFlow
{
class LogicalDevice;

class GraphicsPipeline : public BasePipeline
{
 public:
    explicit GraphicsPipeline(const std::shared_ptr<LogicalDevice>& device,
                              const std::vector<const char*>& shaderFilenames,
                              const PipelineCreateInfo& createInfo);
    ~GraphicsPipeline() override;
    GraphicsPipeline(GraphicsPipeline&& other) noexcept;
    GraphicsPipeline& operator=(GraphicsPipeline&& other) noexcept;

    [[nodiscard]] VkPipelineBindPoint getBindPoint() const noexcept override
    {
        return VK_PIPELINE_BIND_POINT_GRAPHICS;
    }
};
}  // namespace VoxFlow

#endif