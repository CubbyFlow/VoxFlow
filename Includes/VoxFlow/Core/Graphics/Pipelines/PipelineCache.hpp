// Author : snowapril

#ifndef VOXEL_FLOW_PIPELINE_CACHE_HPP
#define VOXEL_FLOW_PIPELINE_CACHE_HPP

#include <volk/volk.h>
#include <VoxFlow/Core/Utils/NonCopyable.hpp>

namespace VoxFlow
{
class PipelineCache : NonCopyable
{
 public:
    explicit PipelineCache();
    ~PipelineCache() override;
    PipelineCache(PipelineCache&& other) noexcept;
    PipelineCache& operator=(PipelineCache&& other) noexcept;

};
}  // namespace VoxFlow

#endif