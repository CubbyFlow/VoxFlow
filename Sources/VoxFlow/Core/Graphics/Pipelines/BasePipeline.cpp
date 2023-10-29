// Author : snowapril

#include <spdlog/spdlog.h>

#include <VoxFlow/Core/Devices/LogicalDevice.hpp>
#include <VoxFlow/Core/Graphics/Pipelines/BasePipeline.hpp>
#include <VoxFlow/Core/Graphics/Pipelines/ShaderUtil.hpp>
#include <VoxFlow/Core/Graphics/Pipelines/PipelineLayout.hpp>
#include <VoxFlow/Core/Graphics/Pipelines/ShaderModule.hpp>
#include <VoxFlow/Core/Graphics/Pipelines/PipelineStreamingContext.hpp>
#include <VoxFlow/Core/Graphics/Pipelines/PipelineCache.hpp>
#include <VoxFlow/Core/Utils/Logger.hpp>

namespace VoxFlow
{
BasePipeline::BasePipeline(PipelineStreamingContext* pipelineStreamingContext,
                           std::vector<ShaderPathInfo>&& shaderFilePaths)
    : _pipelineStreamingContext(pipelineStreamingContext),
      _logicalDevice(pipelineStreamingContext->getLogicalDevice())
{
    for (const ShaderPathInfo& shaderPath : shaderFilePaths)
    {
        _shaderModules.push_back(
            std::make_unique<ShaderModule>(_pipelineStreamingContext, shaderPath));
    }
}

BasePipeline::~BasePipeline()
{
    release();
}

BasePipeline::BasePipeline(BasePipeline&& other) noexcept
{
    operator=(std::move(other));
}

BasePipeline& BasePipeline::operator=(BasePipeline&& other) noexcept
{
    if (&other != this)
    {
        _logicalDevice = other._logicalDevice;
        _pipelineLayout.swap(other._pipelineLayout);
        _shaderModules.swap(other._shaderModules);
        _pipeline = other._pipeline;
        other._pipeline = VK_NULL_HANDLE;
    }
    return *this;
}

void BasePipeline::setPipelineCache(
    std::unique_ptr<PipelineCache>&& pipelineCache)
{
    _pipelineCache = std::move(pipelineCache);
}

void BasePipeline::release()
{
    _shaderModules.clear();
    _pipelineLayout.reset();
    if (_pipeline != VK_NULL_HANDLE)
    {
        vkDestroyPipeline(_logicalDevice->get(), _pipeline, nullptr);
        _pipeline = VK_NULL_HANDLE;
    }
}

void BasePipeline::exportPipelineCache()
{

}

}  // namespace VoxFlow