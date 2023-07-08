// Author : snowapril

#include <VoxFlow/Core/Devices/DeviceQueryContext.hpp>
#include <VoxFlow/Core/Devices/LogicalDevice.hpp>
#include <VoxFlow/Core/Devices/PhysicalDevice.hpp>
#include <VoxFlow/Core/Resources/Buffer.hpp>

namespace VoxFlow
{
DeviceQueryContext::DeviceQueryContext(LogicalDevice* logicalDevice)
    : _logicalDevice(logicalDevice)
{
}

DeviceQueryContext ::~DeviceQueryContext()
{
    release();
}

DeviceQueryContext::DeviceQueryContext(DeviceQueryContext&& context) noexcept
{
    operator=(std::move(context));
}

DeviceQueryContext& DeviceQueryContext::operator=(
    DeviceQueryContext&& context) noexcept
{
    if (this != &context)
    {
        _vkQueryPool = context._vkQueryPool;
        _logicalDevice = context._logicalDevice;
        _queryMode = context._queryMode;
        _timestampPeriod = context._timestampPeriod;
        context._vkQueryPool = VK_NULL_HANDLE;
    }
    return *this;
}

bool DeviceQueryContext::initialize(DeviceQueryMode queryMode)
{
    _queryMode = queryMode;

    VkQueryType queryType;
    switch (_queryMode)
    {
        case DeviceQueryMode::PerformanceCounter:
            queryType = VK_QUERY_TYPE_TIMESTAMP;
            break;
        default:
            VOX_ASSERT(false, "Unknown query mode is given : {}",
                       static_cast<uint32_t>(_queryMode));
            return false;
    }

    VkQueryPoolCreateInfo createInfo = {
        .sType = VK_STRUCTURE_TYPE_QUERY_POOL_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .queryType = queryType,
        .queryCount = kMaxQueryCount,
        .pipelineStatistics = 0,
    };

    VK_ASSERT(vkCreateQueryPool(_logicalDevice->get(), &createInfo, nullptr,
                                &_vkQueryPool));
    if (_vkQueryPool == VK_NULL_HANDLE)
    {
        return false;
    }

#if defined(VK_DEBUG_NAME_ENABLED)
    std::string queryPoolDebugName = fmt::format(
        "QueryPool_QueryMode({})", static_cast<uint32_t>(_queryMode));
    DebugUtil::setObjectName(_logicalDevice, _vkQueryPool,
                             queryPoolDebugName.c_str());
#endif

    const PhysicalDevice* physicalDevice = _logicalDevice->getPhysicalDevice();
    _timestampPeriod =
        physicalDevice->getPhysicalDeviceProperties().limits.timestampPeriod;

    _queryDataBuffer =
        new Buffer("QueryResultBuffer", _logicalDevice,
                   _logicalDevice->getDeviceDefaultResourceMemoryPool());
    if (_queryDataBuffer->makeAllocationResident(BufferInfo{
            ._size = sizeof(uint64_t) * kMaxQueryCount,
            ._usage = BufferUsage::CopyDst | BufferUsage::Readback }) == false)
    {
        return false;
    }

    _mustResetQueryPool = true;
    return true;
}

void DeviceQueryContext::resetQueryPool()
{
    _mustResetQueryPool = true;
}

void DeviceQueryContext::beginTimestampQuery(VkCommandBuffer vkCommandBuffer)
{
    if (_queryMode != DeviceQueryMode::PerformanceCounter)
    {
        return;
    }

    if (_mustResetQueryPool)
    {
        vkCmdResetQueryPool(vkCommandBuffer, _vkQueryPool, 0, kMaxQueryCount);
        _mustResetQueryPool = false;
    }

    VOX_ASSERT((_numTimestamps % 2) == 0, "begin and end call must be in pair");

    vkCmdWriteTimestamp(vkCommandBuffer, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
                        _vkQueryPool, _numTimestamps);
    _numTimestamps++;
}

void DeviceQueryContext::endTimestampQuery(VkCommandBuffer vkCommandBuffer)
{
    if (_queryMode != DeviceQueryMode::PerformanceCounter)
    {
        return;
    }

    VOX_ASSERT((_numTimestamps % 2) == 1, "begin and end call must be in pair");

    vkCmdWriteTimestamp(vkCommandBuffer, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
                        _vkQueryPool, _numTimestamps);
    _numTimestamps++;
}

void DeviceQueryContext::release()
{
    if (_queryDataBuffer != nullptr)
    {
        delete _queryDataBuffer;
    }

    if (_vkQueryPool != VK_NULL_HANDLE)
    {
        vkDestroyQueryPool(_logicalDevice->get(), _vkQueryPool, nullptr);
    }
}
}  // namespace VoxFlow