// Author : snowapril

#ifndef VOXEL_FLOW_PIPELINE_CACHE_HPP
#define VOXEL_FLOW_PIPELINE_CACHE_HPP

#include <volk/volk.h>
#include <VoxFlow/Core/Utils/NonCopyable.hpp>

namespace VoxFlow
{
class PipelineStreamingContext;

class PipelineCache : NonCopyable
{
 public:
    explicit PipelineCache(PipelineStreamingContext* pipelineStreamingContext,
                           const uint32_t pipelineHash);
    ~PipelineCache() override;
    PipelineCache(PipelineCache&& other) noexcept;
    PipelineCache& operator=(PipelineCache&& other) noexcept;

    bool loadPipelineCache(const std::vector<uint8_t>& pipelineCacheBinary);
    void exportPipelineCache();

    [[nodiscard]] inline VkPipelineCache get() const
    {
        return _pipelineCache;
    }

 private:
    PipelineStreamingContext* _pipelineStreamingContext = nullptr;
    LogicalDevice* _logicalDevice = nullptr;
    VkPipelineCache _pipelineCache = VK_NULL_HANDLE;
    uint32_t _pipelineHash = 0;
};
}  // namespace VoxFlow

#endif