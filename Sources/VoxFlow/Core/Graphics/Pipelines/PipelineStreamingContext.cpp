// Author : snowapril

#include <VoxFlow/Core/Devices/LogicalDevice.hpp>
#include <VoxFlow/Core/Graphics/Pipelines/ComputePipeline.hpp>
#include <VoxFlow/Core/Graphics/Pipelines/GraphicsPipeline.hpp>
#include <VoxFlow/Core/Graphics/Pipelines/PipelineStreamingContext.hpp>

namespace VoxFlow
{
PipelineStreamingContext::PipelineStreamingContext(
    LogicalDevice* logicalDevice, const std::string& shaderRootPath)
    : _logicalDevice(logicalDevice), _shaderRootPath(shaderRootPath)
{
}

PipelineStreamingContext::~PipelineStreamingContext()
{
}

PipelineStreamingContext::PipelineStreamingContext(
    PipelineStreamingContext&& other) noexcept
{
    operator=(std::move(other));
}

PipelineStreamingContext& PipelineStreamingContext::operator=(
    PipelineStreamingContext&& other) noexcept
{
    if (this != &other)
    {
        _logicalDevice = other._logicalDevice;
        _registeredPipelines.swap(other._registeredPipelines);
        _shaderRootPath.swap(other._shaderRootPath);
        other._logicalDevice = nullptr;
    }
    return *this;
}

std::shared_ptr<GraphicsPipeline>
PipelineStreamingContext::createGraphicsPipeline(
    std::vector<const char*>&& shaderPaths)
{
    std::vector<std::string> combinedShaderPaths;
    combinedShaderPaths.reserve(shaderPaths.size());
    for (const char* path : shaderPaths)
    {
        combinedShaderPaths.push_back(
            (_shaderRootPath / path).generic_string());
    }

    auto graphicsPipeline = std::make_shared<GraphicsPipeline>(
        _logicalDevice, std::move(combinedShaderPaths));
    _registeredPipelines.push_back(graphicsPipeline);
    graphicsPipeline->setPipelineStreamingContext(this);
    return graphicsPipeline;
}

std::shared_ptr<ComputePipeline>
PipelineStreamingContext::createComputePipeline(const char* shaderPath)
{
    auto computePipeline =
        std::make_shared<ComputePipeline>(_logicalDevice, shaderPath);
    _registeredPipelines.push_back(computePipeline);
    computePipeline->setPipelineStreamingContext(this);
    return computePipeline;
}
}  // namespace VoxFlow