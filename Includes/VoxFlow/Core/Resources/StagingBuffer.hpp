// Author : snowapril

#ifndef VOXEL_FLOW_STAGING_BUFFER_HPP
#define VOXEL_FLOW_STAGING_BUFFER_HPP

#include <vma/include/vk_mem_alloc.h>
#include <volk/volk.h>
#include <VoxFlow/Core/Resources/RenderResource.hpp>
#include <VoxFlow/Core/Utils/FenceObject.hpp>
#include <VoxFlow/Core/Utils/Logger.hpp>
#include <VoxFlow/Core/Utils/NonCopyable.hpp>
#include <VoxFlow/Core/Utils/RendererCommon.hpp>
#include <string_view>

namespace VoxFlow
{
class LogicalDevice;
class RenderResourceMemoryPool;

class StagingBuffer : public RenderResource
{
 public:
    explicit StagingBuffer(std::string_view&& debugName,
                           LogicalDevice* logicalDevice,
                           RenderResourceMemoryPool* renderResourceMemoryPool);
    ~StagingBuffer() override;

 public:
    [[nodiscard]] inline VkBuffer get() const
    {
        return _vkBuffer;
    }

    [[nodiscard]] inline uint32_t getSize() const
    {
        return _size;
    }

    [[nodiscard]] inline RenderResourceType getResourceType() const final
    {
        return RenderResourceType::StagingBuffer;
    }

    // Make the buffer allocation resident if evicted
    bool makeAllocationResident(const uint32_t size);

    // Release buffer object to fence resource manager
    void release();

    /**
     * @return buffer memory mapped address
     */
    [[nodiscard]] uint8_t* map();

    /**
     * unmap permanently mapped address. At now, it just ignore for performance
     * consideration.
     */
    void unmap();

 protected:
 private:
    VkBuffer _vkBuffer = VK_NULL_HANDLE;
    uint32_t _size = 0;
};
}  // namespace VoxFlow

#endif