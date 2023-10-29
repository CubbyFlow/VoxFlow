// Author : snowapril

#ifndef VOXEL_FLOW_PIPELINE_STREAMING_CONTEXT_HPP
#define VOXEL_FLOW_PIPELINE_STREAMING_CONTEXT_HPP

#include <volk/volk.h>
#include <VoxFlow/Core/Utils/NonCopyable.hpp>
#include <memory>
#include <vector>

namespace VoxFlow
{
struct ShaderPathInfo;
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
        std::vector<std::string>&& shaderPaths);

    std::shared_ptr<ComputePipeline> createComputePipeline(
        std::string&& shaderPath);

    bool loadSpirvBinary(std::vector<uint32_t>& outSpirvBinary,
                         const ShaderPathInfo& pathInfo,
                         const bool skipShaderCacheExport = false);

    [[nodiscard]] inline LogicalDevice* getLogicalDevice()
    {
        return _logicalDevice;
    }

 private:
    ShaderPathInfo getShaderPathInfo(const std::string& path);
    bool getPipelineCacheIfExist(std::vector<uint8_t>& outCacheData);
    void exportShaderCache(const ShaderPathInfo& pathInfo,
                           const std::vector<uint32_t>& spirvBinary);

 private:
    LogicalDevice* _logicalDevice;
    std::string _shaderRootPath;
    std::string _shaderCachePath;
    std::string _pipelineCachePath;
    std::vector<std::shared_ptr<BasePipeline>> _registeredPipelines;
};
}  // namespace VoxFlow

#endif