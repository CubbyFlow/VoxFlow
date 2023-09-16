// Author : snowapril

#ifndef VOXEL_FLOW_RESOURCE_BARRIER_MANAGER_HPP
#define VOXEL_FLOW_RESOURCE_BARRIER_MANAGER_HPP

#include <volk/volk.h>
#include <VoxFlow/Core/Utils/NonCopyable.hpp>
#include <VoxFlow/Core/Utils/RendererCommon.hpp>

namespace VoxFlow
{
class CommandBuffer;
class TextureView;
class StagingBufferView;
class BufferView;

class ResourceBarrierManager : private NonCopyable
{
 public:
    ResourceBarrierManager(CommandBuffer* commandBuffer)
        : _commandBuffer(commandBuffer)
    {
    }
    ~ResourceBarrierManager() = default;

 public:
    void addGlobalMemoryBarrier(ResourceAccessMask prevAccessMasks,
                                ResourceAccessMask nextAccessMasks);

    void addTextureMemoryBarrier(TextureView* textureView,
                                 ResourceAccessMask accessMask,
                                 VkPipelineStageFlags nextStageFlags);

    void addBufferMemoryBarrier(BufferView* bufferView,
                                ResourceAccessMask accessMask,
                                VkPipelineStageFlags nextStageFlags);

    void addStagingBufferMemoryBarrier(StagingBufferView* stagingBufferView,
                                       ResourceAccessMask accessMask,
                                       VkPipelineStageFlags nextStageFlags);

    void addExecutionBarrier(VkPipelineStageFlags prevStageFlags,
                             VkPipelineStageFlags nextStageFlags);

    void commitPendingBarriers(const bool inRenderPassScope);

 private:
    struct GlobalMemoryBarrier
    {
        VkAccessFlags _srcAccessFlags = VK_ACCESS_NONE;
        VkAccessFlags _dstAccessFlags = VK_ACCESS_NONE;

        inline bool isValid() const
        {
            return (_srcAccessFlags != VK_ACCESS_NONE) ||
                   (_dstAccessFlags != VK_ACCESS_NONE);
        }

        inline void reset()
        {
            _srcAccessFlags = VK_ACCESS_NONE;
            _dstAccessFlags = VK_ACCESS_NONE;
        }
    } _globalMemoryBarrier;

    struct MemoryBarrierGroup
    {
        std::vector<VkBufferMemoryBarrier> _bufferBarriers;
        std::vector<VkImageMemoryBarrier> _imageBarriers;
        VkPipelineStageFlags _srcStageFlags = VK_PIPELINE_STAGE_NONE;
        VkPipelineStageFlags _dstStageFlags = VK_PIPELINE_STAGE_NONE;

        inline bool isValid() const
        {
            return (_bufferBarriers.size() > 0) || (_imageBarriers.size() > 0);
        }

        inline void reset()
        {
            _bufferBarriers.clear();
            _imageBarriers.clear();
            _srcStageFlags = VK_PIPELINE_STAGE_NONE;
            _dstStageFlags = VK_PIPELINE_STAGE_NONE;
        }
    } _memoryBarrierGroup;

    struct ExecutionBarrier
    {
        VkPipelineStageFlags _srcStageFlags = VK_PIPELINE_STAGE_NONE;
        VkPipelineStageFlags _dstStageFlags = VK_PIPELINE_STAGE_NONE;

        inline bool isValid() const
        {
            return (_srcStageFlags != VK_PIPELINE_STAGE_NONE) ||
                   (_dstStageFlags != VK_PIPELINE_STAGE_NONE);
        }

        inline void reset()
        {
            _srcStageFlags = VK_PIPELINE_STAGE_NONE;
            _dstStageFlags = VK_PIPELINE_STAGE_NONE;
        }
    } _executionBarrier;

    CommandBuffer* _commandBuffer = nullptr;
};
}  // namespace VoxFlow

#endif