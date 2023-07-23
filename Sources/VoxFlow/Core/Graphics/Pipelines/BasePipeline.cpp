// Author : snowapril

#include <spdlog/spdlog.h>

#include <VoxFlow/Core/Devices/LogicalDevice.hpp>
#include <VoxFlow/Core/Graphics/Pipelines/BasePipeline.hpp>
#include <VoxFlow/Core/Graphics/Pipelines/GlslangUtil.hpp>
#include <VoxFlow/Core/Graphics/Pipelines/PipelineLayout.hpp>
#include <VoxFlow/Core/Graphics/Pipelines/ShaderModule.hpp>
#include <VoxFlow/Core/Utils/Logger.hpp>

namespace VoxFlow
{
BasePipeline::BasePipeline(LogicalDevice* logicalDevice,
                           std::initializer_list<const char*>&& shaderFilePaths)
    : _logicalDevice(logicalDevice)
{
    for (const char* shaderPath : shaderFilePaths)
    {
        _shaderModules.push_back(
            std::make_unique<ShaderModule>(_logicalDevice, shaderPath));
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

bool BasePipeline::initializePipelineLayout()
{
    _pipelineLayout = std::make_unique<PipelineLayout>(_logicalDevice);

    std::vector<PipelineLayoutDescriptor> combinedLayoutBindings;

    const size_t numShaderModules = _shaderModules.size();
    combinedLayoutBindings.reserve(numShaderModules);

    for (size_t i = 0; i < numShaderModules; ++i)
    {
        combinedLayoutBindings.push_back(
            _shaderModules[i]->getShaderLayoutBinding());
    }

    return _pipelineLayout->initialize(std::move(combinedLayoutBindings));
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

}  // namespace VoxFlow