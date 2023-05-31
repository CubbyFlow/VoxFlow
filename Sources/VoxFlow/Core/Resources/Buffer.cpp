// Author : snowapril

#include <VoxFlow/Core/Devices/LogicalDevice.hpp>
#include <VoxFlow/Core/Resources/Buffer.hpp>
#include <VoxFlow/Core/Resources/RenderResourceGarbageCollector.hpp>
#include <VoxFlow/Core/Resources/RenderResourceMemoryPool.hpp>
#include <VoxFlow/Core/Utils/DebugUtil.hpp>
#include <VoxFlow/Core/Utils/Logger.hpp>

namespace VoxFlow
{
static VkBufferUsageFlags convertToVkBufferUsage(BufferUsage usage)
{
    VkBufferUsageFlags resultUsage = 0;
    if (usage & BufferUsage::ConstantBuffer)
        resultUsage |= VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
    if (usage & BufferUsage::RwStructuredBuffer)
        resultUsage |= VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
    if (usage & BufferUsage::VertexBuffer)
        resultUsage |= VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    if (usage & BufferUsage::IndexBuffer)
        resultUsage |= VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
    if (usage & BufferUsage::IndirectCommand)
        resultUsage |= VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT;
    if (usage & BufferUsage::CopyDst)
        resultUsage |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;
    if (usage & BufferUsage::CopySrc)
        resultUsage |= VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    return resultUsage;
}

Buffer::Buffer(std::string&& debugName, LogicalDevice* logicalDevice,
               RenderResourceMemoryPool* renderResourceMemoryPool)
    : _debugName(std::move(debugName)),
      _logicalDevice(logicalDevice),
      _renderResourceMemoryPool(renderResourceMemoryPool)
{
}
Buffer::~Buffer()
{
}

bool Buffer::makeAllocationResident(const BufferInfo& bufferInfo)
{
    VOX_ASSERT(bufferInfo._usage != BufferUsage::Unknown,
               "BufferUsage must be specified");

    _bufferInfo = bufferInfo;
    VkBufferCreateInfo bufferCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .size = bufferInfo._size,
        .usage = convertToVkBufferUsage(bufferInfo._usage),
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .queueFamilyIndexCount = 0,
        .pQueueFamilyIndices = nullptr
    };

    VmaAllocationCreateInfo vmaCreateInfo = {
        .flags =
            VMA_ALLOCATION_CREATE_STRATEGY_MIN_MEMORY_BIT,  // TODO(snowapril) :
                                                            // choose best one
        .usage = VMA_MEMORY_USAGE_AUTO,
        .requiredFlags = 0,
        .preferredFlags = 0,
        .memoryTypeBits = 0,
        .pool = VK_NULL_HANDLE,
        .pUserData = nullptr
    };

    VK_ASSERT(vmaCreateBuffer(_renderResourceMemoryPool->get(),
                              &bufferCreateInfo, &vmaCreateInfo, &_vkBuffer,
                              &_bufferAllocation, nullptr));

    if (_vkBuffer == VK_NULL_HANDLE)
    {
        VOX_ASSERT(false, "Failed to initialize buffer({})", _debugName);
        return false;
    }

#if defined(VK_DEBUG_NAME_ENABLED)
    DebugUtil::setObjectName(_logicalDevice, _vkBuffer, _debugName.c_str());
#endif
    return true;
}

std::optional<uint32_t> Buffer::createBufferView(const BufferViewInfo& viewInfo)
{
    const uint32_t viewIndex = static_cast<uint32_t>(_ownedBufferViews.size());
    std::shared_ptr<BufferView> bufferView = std::make_shared<BufferView>(
        fmt::format("{}_View({})", _debugName, viewIndex), _logicalDevice,
        weak_from_this());

    if (bufferView->initialize(viewInfo) == false)
    {
        return {};
    }

    _ownedBufferViews.push_back(std::move(bufferView));
    return viewIndex;
}

void Buffer::release()
{
    _ownedBufferViews.clear();

    if (_vkBuffer != VK_NULL_HANDLE)
    {
        RenderResourceGarbageCollector::Get().pushRenderResourceGarbage(
            RenderResourceGarbage(std::move(_accessedFences), [this]() {
                vmaDestroyBuffer(_renderResourceMemoryPool->get(), _vkBuffer,
                                 _bufferAllocation);
            }));
    }
}

BufferView::BufferView(std::string&& debugName, LogicalDevice* logicalDevice,
                       std::weak_ptr<Buffer>&& ownerBuffer)
    : BindableResourceView(std::move(debugName), logicalDevice),
      _ownerBuffer(std::move(ownerBuffer))
{
}

BufferView::~BufferView()
{
    release();
}

bool BufferView::initialize(const BufferViewInfo& viewInfo)
{
    _bufferViewInfo = viewInfo;
    return true;
}

void BufferView::release()
{
}

VkDescriptorBufferInfo BufferView::getDescriptorBufferInfo() const
{
    std::shared_ptr<Buffer> ownerBuffer = _ownerBuffer.lock();

    return VkDescriptorBufferInfo{
        .buffer = ownerBuffer == nullptr ? VK_NULL_HANDLE : ownerBuffer->get(),
        .offset = _bufferViewInfo._offset,
        .range = _bufferViewInfo._range,
    };
}
}  // namespace VoxFlow