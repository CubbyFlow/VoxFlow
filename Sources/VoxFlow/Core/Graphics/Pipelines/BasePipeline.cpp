// Author : snowapril

#include <glslang_c_interface.h>
#include <spdlog/spdlog.h>
#include <VoxFlow/Core/Devices/LogicalDevice.hpp>
#include <VoxFlow/Core/Graphics/Pipelines/BasePipeline.hpp>
#include <VoxFlow/Core/Utils/Logger.hpp>
#include <VoxFlow/Core/Utils/pch.hpp>
#include <fstream>
#include <string_view>

namespace VoxFlow
{
glslang_stage_t getStageFromFilename(const std::string_view filename);
bool readShaderFile(const std::string& filename, std::vector<char>* dst);

BasePipeline::BasePipeline(const std::shared_ptr<LogicalDevice>& device)
    : _device(device)
{
    // Do nothing
}

BasePipeline::~BasePipeline()
{
    release();
}

BasePipeline::BasePipeline(BasePipeline&& other) noexcept
    : _device(std::move(other._device)), _pipeline(other._pipeline)
{
    // Do nothing
}

BasePipeline& BasePipeline::operator=(BasePipeline&& other) noexcept
{
    if (&other != this)
    {
        _device = std::move(other._device);
        _pipeline = other._pipeline;
    }
    return *this;
}

void BasePipeline::release()
{
    if (_pipeline != VK_NULL_HANDLE)
    {
        vkDestroyPipeline(_device->get(), _pipeline, nullptr);
        _pipeline = VK_NULL_HANDLE;
    }
}

glslang_stage_t getStageFromFilename(const std::string_view filename)
{
    if (filename.ends_with(".vert"))
        return GLSLANG_STAGE_VERTEX;
    if (filename.ends_with(".frag"))
        return GLSLANG_STAGE_FRAGMENT;
    if (filename.ends_with(".geom"))
        return GLSLANG_STAGE_GEOMETRY;
    if (filename.ends_with(".comp"))
        return GLSLANG_STAGE_COMPUTE;
    if (filename.ends_with(".tesc"))
        return GLSLANG_STAGE_TESSCONTROL;
    if (filename.ends_with(".tese"))
        return GLSLANG_STAGE_TESSEVALUATION;

    ABORT_WITH_MSG("Undefined shader extension is given");
    return GLSLANG_STAGE_COUNT;
}

bool readShaderFile(const std::string& filename, std::vector<char>* dst)
{
    std::ifstream file(filename, std::ios::in | std::ios::ate);
    if (!file.is_open())
    {
        ABORT_WITH_MSG("Failed to find shader file {}", filename);
        return false;
    }

    const size_t fileSize = file.tellg();
    dst->resize(fileSize);

    file.seekg(std::ios::beg);
    file.read(dst->data(), static_cast<std::streamsize>(fileSize));

    file.close();
    return true;
}
}  // namespace VoxFlow