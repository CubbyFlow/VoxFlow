// Author : snowapril

#ifndef VOXEL_FLOW_PIPELINE_STREAMING_CONTEXT_HPP
#define VOXEL_FLOW_PIPELINE_STREAMING_CONTEXT_HPP

#include <volk/volk.h>
#include <VoxFlow/Core/Utils/NonCopyable.hpp>
#include <filesystem>
#include <memory>
#include <vector>

namespace VoxFlow
{
class LogicalDevice;
class BasePipeline;
class GraphicsPipeline;
class ComputePipeline;

class PipelineStreamingContext : NonCopyable
{
 public:
    explicit PipelineStreamingContext(LogicalDevice* logicalDevice,
                                      const std::string& shaderRootPath);
    ~PipelineStreamingContext() override;
    PipelineStreamingContext(PipelineStreamingContext&& other) noexcept;
    PipelineStreamingContext& operator=(
        PipelineStreamingContext&& other) noexcept;

 public:
    std::shared_ptr<GraphicsPipeline> createGraphicsPipeline(
        std::vector<const char*>&& shaderPaths);

    std::shared_ptr<ComputePipeline> createComputePipeline(
        const char* shaderPath);

 private:
    LogicalDevice* _logicalDevice;
    std::filesystem::path _shaderRootPath;
    std::vector<std::shared_ptr<BasePipeline>> _registeredPipelines;
};
}  // namespace VoxFlow

#endif