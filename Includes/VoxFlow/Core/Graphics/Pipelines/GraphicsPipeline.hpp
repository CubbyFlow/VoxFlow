// Author : snowapril

#ifndef VOXEL_FLOW_GRAPHICS_PIPELINE_HPP
#define VOXEL_FLOW_GRAPHICS_PIPELINE_HPP

#include <VoxFlow/Core/Graphics/Pipelines/BasePipeline.hpp>

namespace VoxFlow
{
class LogicalDevice;

class GraphicsPipeline : BasePipeline
{
 public:
    explicit GraphicsPipeline(const std::shared_ptr<LogicalDevice>& device);
    ~GraphicsPipeline() override;
    GraphicsPipeline(GraphicsPipeline&& other) noexcept;
    GraphicsPipeline& operator=(GraphicsPipeline&& other) noexcept;
};
}  // namespace VoxFlow

#endif