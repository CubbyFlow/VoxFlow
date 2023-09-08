// Author : snowapril

#ifndef VOXEL_FLOW_RESOURCE_BARRIER_MANAGER_HPP
#define VOXEL_FLOW_RESOURCE_BARRIER_MANAGER_HPP

#include <volk/volk.h>
#include <VoxFlow/Core/Utils/NonCopyable.hpp>
#include <VoxFlow/Core/Utils/RendererCommon.hpp>

namespace VoxFlow
{
class BindableResourceView;

class ResourceBarrierManager : private NonCopyable
{
 public:
    ResourceBarrierManager() = default;
    ~ResourceBarrierManager() = default;

 public:
    void addGlobalMemoryBarrier(ResourceAccessMask prevAccessMasks,
                                ResourceAccessMask nextAccessMasks);

    void addMemoryBarrier(BindableResourceView* view,
                          ResourceAccessMask accessMask);

    void addExecutionBarrier(VkShaderStageFlagBits prevStageBits,
                             VkShaderStageFlagBits nextStageBits);

    void commitPendingBarriers(VkCommandBuffer commandBuffer, const bool inRenderPassScope);

 private:
    struct GlobalMemoryBarrier
    {
        VkAccessFlags _srcAccessFlags = VK_ACCESS_NONE;
        VkAccessFlags _dstAccessFlags = VK_ACCESS_NONE;
        VkShaderStageFlagBits _srcStageFlags =
            VK_SHADER_STAGE_FLAG_BITS_MAX_ENUM;
        VkShaderStageFlagBits _dstStageFlags =
            VK_SHADER_STAGE_FLAG_BITS_MAX_ENUM;

        inline bool isValid() const
        {
            return (_srcAccessFlags != VK_ACCESS_NONE) &&
                   (_dstAccessFlags != VK_ACCESS_NONE) &&
                   (_srcStageFlags != VK_SHADER_STAGE_FLAG_BITS_MAX_ENUM) &&
                   (_dstStageFlags != VK_SHADER_STAGE_FLAG_BITS_MAX_ENUM);
        }

        inline void reset()
        {
            _srcAccessFlags = VK_ACCESS_NONE;
            _dstAccessFlags = VK_ACCESS_NONE;
            _srcStageFlags = VK_SHADER_STAGE_FLAG_BITS_MAX_ENUM;
            _dstStageFlags = VK_SHADER_STAGE_FLAG_BITS_MAX_ENUM;
        }
    } _globalMemoryBarrier;

    struct MemoryBarrierGroup
    {
        std::vector<VkBufferMemoryBarrier> _bufferBarriers;
        std::vector<VkImageMemoryBarrier> _imageBarriers;
        VkShaderStageFlagBits _srcStageFlags =
            VK_SHADER_STAGE_FLAG_BITS_MAX_ENUM;
        VkShaderStageFlagBits _dstStageFlags =
            VK_SHADER_STAGE_FLAG_BITS_MAX_ENUM;

        inline bool isValid() const
        {
            return (_bufferBarriers.size() > 0) &&
                   (_imageBarriers.size() > 0) &&
                   (_srcStageFlags != VK_SHADER_STAGE_FLAG_BITS_MAX_ENUM) &&
                   (_dstStageFlags != VK_SHADER_STAGE_FLAG_BITS_MAX_ENUM);
        }

        inline void reset()
        {
            _bufferBarriers.clear();
            _imageBarriers.clear();
            _srcStageFlags = VK_SHADER_STAGE_FLAG_BITS_MAX_ENUM;
            _dstStageFlags = VK_SHADER_STAGE_FLAG_BITS_MAX_ENUM;
        }
    } _memoryBarrierGroup;

    struct ExecutionBarrier
    {
        VkShaderStageFlagBits _srcStageFlags =
            VK_SHADER_STAGE_FLAG_BITS_MAX_ENUM;
        VkShaderStageFlagBits _dstStageFlags =
            VK_SHADER_STAGE_FLAG_BITS_MAX_ENUM;

        inline bool isValid() const
        {
            return (_srcStageFlags != VK_SHADER_STAGE_FLAG_BITS_MAX_ENUM) &&
                   (_dstStageFlags != VK_SHADER_STAGE_FLAG_BITS_MAX_ENUM);
        }

        inline void reset()
        {
            _srcStageFlags = VK_SHADER_STAGE_FLAG_BITS_MAX_ENUM;
            _dstStageFlags = VK_SHADER_STAGE_FLAG_BITS_MAX_ENUM;
        }
    } _executionBarrier;
};
}  // namespace VoxFlow

#endif