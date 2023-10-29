// Author : snowapril

#include <VoxFlow/Core/Devices/LogicalDevice.hpp>
#include <VoxFlow/Core/Graphics/Pipelines/ComputePipeline.hpp>
#include <VoxFlow/Core/Graphics/Pipelines/GraphicsPipeline.hpp>
#include <VoxFlow/Core/Graphics/Pipelines/PipelineCache.hpp>
#include <VoxFlow/Core/Graphics/Pipelines/PipelineStreamingContext.hpp>
#include <VoxFlow/Core/Graphics/Pipelines/ShaderModule.hpp>
#include <VoxFlow/Core/Utils/HashUtil.hpp>
#include <filesystem>
#include <fstream>
#include <string_view>

namespace VoxFlow
{
PipelineStreamingContext::PipelineStreamingContext(LogicalDevice* logicalDevice, const std::string& shaderRootPath)
    : _logicalDevice(logicalDevice),
      _shaderRootPath(shaderRootPath),
      _shaderCachePath(_shaderRootPath + "ShaderCache/"),
      _pipelineCachePath(_shaderRootPath + "PipelineCache/")
{
    std::filesystem::create_directory(_shaderCachePath);
    std::filesystem::create_directory(_pipelineCachePath);
}

PipelineStreamingContext::~PipelineStreamingContext()
{
}

PipelineStreamingContext::PipelineStreamingContext(PipelineStreamingContext&& other) noexcept
{
    operator=(std::move(other));
}

PipelineStreamingContext& PipelineStreamingContext::operator=(PipelineStreamingContext&& other) noexcept
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

std::shared_ptr<GraphicsPipeline> PipelineStreamingContext::createGraphicsPipeline(std::vector<std::string>&& shaderPaths)
{
    uint32_t pipelineHash = 0;

    std::vector<ShaderPathInfo> pathInfos;
    pathInfos.reserve(shaderPaths.size());
    for (std::string path : shaderPaths)
    {
        pathInfos.push_back(getShaderPathInfo(path));
        hash_combine(pipelineHash, path);
    }

    auto graphicsPipeline = std::make_shared<GraphicsPipeline>(this, std::move(pathInfos));

    auto pipelineCache = std::make_unique<PipelineCache>(this, pipelineHash);

    std::vector<uint8_t> pipelineCacheBinary;
    const std::string pipelineCachePath = _pipelineCachePath + std::to_string(pipelineHash) + ".vfpipeline";
    getPipelineCacheIfExist(pipelineCachePath, pipelineCacheBinary);

    if (pipelineCacheBinary.size() > 0)
    {
        spdlog::info("pipeline cache loaded [ {} ]", pipelineCachePath);
    }

    pipelineCache->loadPipelineCache(pipelineCacheBinary);
    graphicsPipeline->setPipelineCache(std::move(pipelineCache));

    _registeredPipelines.push_back(graphicsPipeline);
    return graphicsPipeline;
}

std::shared_ptr<ComputePipeline> PipelineStreamingContext::createComputePipeline(std::string&& shaderPath)
{
    uint32_t pipelineHash = 0;
    hash_combine(pipelineHash, shaderPath);

    auto computePipeline = std::make_shared<ComputePipeline>(this, getShaderPathInfo(shaderPath));

    auto pipelineCache = std::make_unique<PipelineCache>(this, pipelineHash);

    std::vector<uint8_t> pipelineCacheBinary;
    const std::string pipelineCachePath = _pipelineCachePath + std::to_string(pipelineHash) + ".vfpipeline";
    getPipelineCacheIfExist(pipelineCachePath, pipelineCacheBinary);

    if (pipelineCacheBinary.size() > 0)
    {
        spdlog::info("pipeline cache loaded [ {} ]", pipelineCachePath);
    }

    pipelineCache->loadPipelineCache(pipelineCacheBinary);
    computePipeline->setPipelineCache(std::move(pipelineCache));

    _registeredPipelines.push_back(computePipeline);
    return computePipeline;
}

bool PipelineStreamingContext::loadSpirvBinary(std::vector<uint32_t>& outSpirvBinary, const ShaderPathInfo& pathInfo, const bool skipShaderCacheExport)
{
    bool compileResult = true;
    if (pathInfo.fileType == ShaderFileType::Glsl)
    {
        std::string shaderAbsPath = _shaderRootPath + pathInfo.path;

        std::vector<char> shaderSource;
        compileResult = GlslangUtil::ReadShaderFile(shaderAbsPath.c_str(), &shaderSource);

        if (compileResult == false)
            return false;

        compileResult = GlslangUtil::CompileShader(GlslangUtil::GlslangStageFromFilename(shaderAbsPath), shaderSource.data(), &outSpirvBinary);

        if (skipShaderCacheExport == false)
        {
            exportShaderCache(pathInfo, outSpirvBinary);
        }
    }
    else if (pathInfo.fileType == ShaderFileType::Spirv)
    {
        std::string shaderAbsPath = _shaderCachePath + pathInfo.path;

        compileResult = ShaderUtil::ReadSpirvBinary(shaderAbsPath.c_str(), &outSpirvBinary);

        spdlog::info("shader cache loaded [ {} ]", shaderAbsPath);
    }

    return compileResult;
}

void PipelineStreamingContext::exportShaderCache(const ShaderPathInfo& pathInfo, const std::vector<uint32_t>& spirvBinary)
{
    std::ofstream shaderCacheFile;

    std::string shaderCachePath = _shaderCachePath + pathInfo.path.substr(0, pathInfo.path.find_last_of('.')) + ".vfcache_" +
                                  ShaderUtil::ConvertToShaderFileExtension(pathInfo.shaderStage);

    shaderCacheFile.open(shaderCachePath, std::ios::app | std::ios::binary);
    shaderCacheFile.write(reinterpret_cast<const char*>(spirvBinary.data()), spirvBinary.size() * 4);
    shaderCacheFile.close();

    spdlog::info("shader cache exported [ {} ]", shaderCachePath);
}

void PipelineStreamingContext::exportPipelineCache(const size_t pipelineHash, std::vector<uint8_t>&& pipelineCacheBinary)
{
    std::ofstream pipelineCacheFile;

    std::string pipelineCachePath = _pipelineCachePath + std::to_string(pipelineHash) + ".vfpipeline";

    pipelineCacheFile.open(pipelineCachePath, std::ios::app | std::ios::binary);
    pipelineCacheFile.write(reinterpret_cast<const char*>(pipelineCacheBinary.data()), pipelineCacheBinary.size() * 4);
    pipelineCacheFile.close();

    spdlog::info("pipeline cache exported [ {} ]", pipelineCachePath);
}

ShaderPathInfo PipelineStreamingContext::getShaderPathInfo(const std::string& path)
{
    ShaderPathInfo pathInfo;
    pathInfo.path = path;
    pathInfo.fileType = ShaderFileType::Glsl;

    const std::string extension = path.substr(path.find_last_of('.') + 1);
    if (extension == "vert")
    {
        pathInfo.shaderStage = ShaderStage::Vertex;
    }
    else if (extension == "frag")
    {
        pathInfo.shaderStage = ShaderStage::Fragment;
    }
    else if (extension == "comp")
    {
        pathInfo.shaderStage = ShaderStage::Compute;
    }

    const std::string shaderCacheName = pathInfo.path.substr(0, pathInfo.path.find_last_of('.')) + ".vfcache_" + extension;

    if (std::filesystem::exists(_shaderCachePath + shaderCacheName))
    {
        pathInfo.path = shaderCacheName;
        pathInfo.fileType = ShaderFileType::Spirv;
    }

    return pathInfo;
}

void PipelineStreamingContext::getPipelineCacheIfExist(const std::string& pipelineCachePath, std::vector<uint8_t>& outCacheData)
{
    std::ifstream file(pipelineCachePath, std::ios::in | std::ios::ate | std::ios::binary);
    if (!file.is_open())
    {
        return;
    }

    const size_t fileSize = file.tellg();
    outCacheData.resize(fileSize);

    file.seekg(std::ios::beg);
    file.read(reinterpret_cast<char*>(outCacheData.data()), fileSize);

    file.close();
}

}  // namespace VoxFlow