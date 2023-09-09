// Author : snowapril

#ifndef VOXEL_FLOW_STAGING_BUFFER_HPP
#define VOXEL_FLOW_STAGING_BUFFER_HPP

#include <vma/include/vk_mem_alloc.h>
#include <volk/volk.h>
#include <VoxFlow/Core/Resources/RenderResource.hpp>
#include <VoxFlow/Core/Resources/ResourceView.hpp>
#include <VoxFlow/Core/Utils/FenceObject.hpp>
#include <VoxFlow/Core/Utils/Logger.hpp>
#include <VoxFlow/Core/Utils/NonCopyable.hpp>
#include <VoxFlow/Core/Utils/RendererCommon.hpp>
#include <string_view>

namespace VoxFlow
{
class LogicalDevice;
class RenderResourceMemoryPool;
class StagingBufferView;

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

    [[nodiscard]] inline std::shared_ptr<StagingBufferView> getView(
        const uint32_t viewIndex) const
    {
        VOX_ASSERT(viewIndex < static_cast<uint32_t>(_ownedBufferViews.size()),
                   "Given Index ({}), Num Buffer Views ({})", viewIndex,
                   _ownedBufferViews.size());
        return _ownedBufferViews[viewIndex];
    }

    [[nodiscard]] inline RenderResourceType getResourceType() const final
    {
        return RenderResourceType::StagingBuffer;
    }

    // Make the buffer allocation resident if evicted
    bool makeAllocationResident(const uint32_t size);

    // Release buffer object to fence resource manager
    void release();

    // Create buffer view and return its index for given buffer view info
    std::optional<uint32_t> createStagingBufferView(
        const BufferViewInfo& viewInfo);

    // Get default created view that is pointing whole buffer
    [[nodiscard]] inline StagingBufferView* getDefaultView() const
    {
        return _defaultView;
    }

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
    std::vector<std::shared_ptr<StagingBufferView>> _ownedBufferViews;
    StagingBufferView* _defaultView = nullptr;
};

class StagingBufferView : public ResourceView
{
 public:
    explicit StagingBufferView(std::string&& debugName, LogicalDevice* logicalDevice,
                        RenderResource* ownerResource);
    ~StagingBufferView();

 public:
    [[nodiscard]] inline BufferViewInfo getViewInfo() const
    {
        return _bufferViewInfo;
    }

    bool initialize(const BufferViewInfo& viewInfo);

    void release();

    // Returns VkDescriptorBufferInfo for this buffer view object.
    VkDescriptorBufferInfo getDescriptorBufferInfo() const;

    ResourceViewType getResourceViewType() const final
    {
        return ResourceViewType::StagingBufferView;
    }

 protected:
 private:
    BufferViewInfo _bufferViewInfo;
};
}  // namespace VoxFlow

#endif