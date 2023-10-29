// Author : snowapril

#include <VoxFlow/Core/Devices/LogicalDevice.hpp>
#include <VoxFlow/Core/Graphics/Pipelines/PipelineCache.hpp>
#include <VoxFlow/Core/Graphics/Pipelines/PipelineStreamingContext.hpp>

namespace VoxFlow
{
PipelineCache::PipelineCache(PipelineStreamingContext* pipelineStreamingContext, const uint32_t pipelineHash)
    : _pipelineStreamingContext(pipelineStreamingContext), _logicalDevice(pipelineStreamingContext->getLogicalDevice()), _pipelineHash(pipelineHash)
{
}

PipelineCache::~PipelineCache()
{
    exportPipelineCache();
}

PipelineCache::PipelineCache(PipelineCache&& other) noexcept
{
    operator=(std::move(other));
}

PipelineCache& PipelineCache::operator=(PipelineCache&& other) noexcept
{
    if (this != &other)
    {
        _pipelineCache = other._pipelineCache;
        _pipelineStreamingContext = other._pipelineStreamingContext;
        _logicalDevice = other._logicalDevice;

        other._pipelineCache = VK_NULL_HANDLE;
        other._pipelineStreamingContext = nullptr;
        other._logicalDevice = nullptr;
    }
    return *this;
}

bool PipelineCache::loadPipelineCache(const std::vector<uint8_t>& pipelineCacheBinary)
{
    VkPipelineCacheCreateInfo cacheCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .initialDataSize = static_cast<uint32_t>(pipelineCacheBinary.size() * sizeof(uint8_t)),
        .pInitialData = pipelineCacheBinary.data(),
    };

    VkResult result = vkCreatePipelineCache(_logicalDevice->get(), &cacheCreateInfo, nullptr, &_pipelineCache);

    VK_ASSERT(result);

    return result == VK_SUCCESS;
}

void PipelineCache::exportPipelineCache()
{
    if (_pipelineCache != VK_NULL_HANDLE)
    {
        size_t size{};
        VK_ASSERT(vkGetPipelineCacheData(_logicalDevice->get(), _pipelineCache, &size, nullptr));

        std::vector<uint8_t> pipelineCacheBinary(size);
        VK_ASSERT(vkGetPipelineCacheData(_logicalDevice->get(), _pipelineCache, &size, pipelineCacheBinary.data()));

        _pipelineStreamingContext->exportPipelineCache(_pipelineHash, std::move(pipelineCacheBinary));

        vkDestroyPipelineCache(_logicalDevice->get(), _pipelineCache, nullptr);
        _pipelineCache = VK_NULL_HANDLE;
    }
}

}  // namespace VoxFlow