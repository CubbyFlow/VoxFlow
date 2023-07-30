// Author : snowapril

#ifndef VOXEL_FLOW_BUFFER_HPP
#define VOXEL_FLOW_BUFFER_HPP

#include <volk/volk.h>
#include <vma/include/vk_mem_alloc.h>
#include <VoxFlow/Core/Resources/BindableResourceView.hpp>
#include <VoxFlow/Core/Utils/FenceObject.hpp>
#include <VoxFlow/Core/Utils/Logger.hpp>
#include <VoxFlow/Core/Utils/NonCopyable.hpp>
#include <VoxFlow/Core/Utils/RendererCommon.hpp>
#include <string>

namespace VoxFlow
{
class LogicalDevice;
class RenderResourceMemoryPool;
class BufferView;

class Buffer : private NonCopyable, public std::enable_shared_from_this<Buffer>
{
 public:
    explicit Buffer(std::string&& debugName, LogicalDevice* logicalDevice,
                    RenderResourceMemoryPool* renderResourceMemoryPool);
    ~Buffer();

 public:
    [[nodiscard]] inline VkBuffer get() const
    {
        return _vkBuffer;
    }

    [[nodiscard]] inline std::shared_ptr<BufferView> getView(
        const uint32_t viewIndex) const
    {
        VOX_ASSERT(viewIndex < static_cast<uint32_t>(_ownedBufferViews.size()),
                   "Given Index ({}), Num Buffer Views ({})", viewIndex,
                   _ownedBufferViews.size());
        return _ownedBufferViews[viewIndex];
    }

    inline BufferInfo getBufferInfo() const
    {
        return _bufferInfo;
    }

    // Make the buffer allocation resident if evicted
    bool makeAllocationResident(const BufferInfo& bufferInfo);

    // Create buffer view and return its index for given buffer view info
    std::optional<uint32_t> createBufferView(const BufferViewInfo& viewInfo);

    // Release buffer object to fence resource manager
    void release();

    /**
     * @param data buffer data address to be uploaded
     * @param offset 
     * @param size
     * @return whether upload is success or not
     */
    bool upload(const void* data, const uint32_t offset, const uint32_t size);

    /**
     * @return buffer memory mapped address
     */
    [[nodiscard]] void* map();

    /**
     * unmap permanently mapped address. At now, it just ignore for performance consideration.
     */
    void unmap();

 protected:
 private:
    std::string _debugName;
    LogicalDevice* _logicalDevice = nullptr;
    RenderResourceMemoryPool* _renderResourceMemoryPool = nullptr;
    VkBuffer _vkBuffer = VK_NULL_HANDLE;
    VmaAllocation _bufferAllocation = nullptr;
    BufferInfo _bufferInfo;

    std::vector<std::shared_ptr<BufferView>> _ownedBufferViews;
    std::vector<FenceObject> _accessedFences;

    void* _permanentMappedAddress = nullptr;
};

class BufferView : public BindableResourceView
{
 public:
    explicit BufferView(std::string&& debugName, LogicalDevice* logicalDevice,
                        std::weak_ptr<Buffer>&& ownerBuffer);
    ~BufferView();

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
        return ResourceViewType::BufferView;
    }

 protected:
 private:
    std::weak_ptr<Buffer> _ownerBuffer;
    BufferViewInfo _bufferViewInfo;
};
}  // namespace VoxFlow

#endif