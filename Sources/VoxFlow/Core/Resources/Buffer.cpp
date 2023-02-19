// Author : snowapril

#include <VoxFlow/Core/Resources/Buffer.hpp>
#include <VoxFlow/Core/Resources/RenderResourceMemoryPool.hpp>
#include <VoxFlow/Core/Devices/LogicalDevice.hpp>
#include <VoxFlow/Core/Utils/Logger.hpp>
#include <VoxFlow/Core/Utils/DebugUtil.hpp>

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

bool Buffer::initialize(const BufferInfo& bufferInfo)
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

    DebugUtil::setObjectName(_logicalDevice, _vkBuffer, _debugName.c_str());
    return true;
}

std::optional<uint32_t> Buffer::createBufferView(const BufferViewInfo& viewInfo)
{
    VkBufferViewCreateInfo viewCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_BUFFER_VIEW_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .buffer = _vkBuffer,
        .format = viewInfo._format,
        .offset = viewInfo._offset,
        .range = viewInfo._range
    };
    VkBufferView vkBufferView = VK_NULL_HANDLE;
    VK_ASSERT(vkCreateBufferView(_logicalDevice->get(), &viewCreateInfo,
                                 nullptr, &vkBufferView));

    const uint32_t viewIndex = static_cast<uint32_t>(_vkBufferViews.size());
    const std::string& viewDebugName =
        fmt::format("%s_View({})", _debugName, viewIndex);

    if (vkBufferView == VK_NULL_HANDLE)
    {
        VOX_ASSERT(false, "Failed to create BufferView({})", viewDebugName);
        return {};
    }

    _vkBufferViews.emplace_back(vkBufferView, viewInfo);
    return viewIndex;
}

void Buffer::release()
{
    for (auto& viewPair : _vkBufferViews)
    {
        vkDestroyBufferView(_logicalDevice->get(), viewPair.first, nullptr);
    }
    _vkBufferViews.clear();

    if (_vkBuffer != VK_NULL_HANDLE)
    {
        vmaDestroyBuffer(_renderResourceMemoryPool->get(), _vkBuffer,
                         _bufferAllocation);
    }
}
}  // namespace VoxFlow