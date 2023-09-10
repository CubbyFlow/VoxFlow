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
                                 VkShaderStageFlags nextStageFlags);

    void addBufferMemoryBarrier(BufferView* bufferView,
                                ResourceAccessMask accessMask,
                                VkShaderStageFlags nextStageFlags);

    void addStagingBufferMemoryBarrier(StagingBufferView* stagingBufferView,
                                       ResourceAccessMask accessMask,
                                       VkShaderStageFlags nextStageFlags);

    void addExecutionBarrier(VkShaderStageFlags prevStageFlags,
                             VkShaderStageFlags nextStageFlags);

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
        VkShaderStageFlags _srcStageFlags = VK_SHADER_STAGE_FLAG_BITS_MAX_ENUM;
        VkShaderStageFlags _dstStageFlags = VK_SHADER_STAGE_FLAG_BITS_MAX_ENUM;

        inline bool isValid() const
        {
            return (_bufferBarriers.size() > 0) ||
                   (_imageBarriers.size() > 0);
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
        VkShaderStageFlags _srcStageFlags = VK_SHADER_STAGE_FLAG_BITS_MAX_ENUM;
        VkShaderStageFlags _dstStageFlags = VK_SHADER_STAGE_FLAG_BITS_MAX_ENUM;

        inline bool isValid() const
        {
            return (_srcStageFlags != VK_SHADER_STAGE_FLAG_BITS_MAX_ENUM) ||
                   (_dstStageFlags != VK_SHADER_STAGE_FLAG_BITS_MAX_ENUM);
        }

        inline void reset()
        {
            _srcStageFlags = VK_SHADER_STAGE_FLAG_BITS_MAX_ENUM;
            _dstStageFlags = VK_SHADER_STAGE_FLAG_BITS_MAX_ENUM;
        }
    } _executionBarrier;

    CommandBuffer* _commandBuffer = nullptr;
};
}  // namespace VoxFlow

#endif