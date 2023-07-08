// Author : snowapril

#ifndef VOXEL_FLOW_DEVICE_QUERY_CONTEXT_HPP
#define VOXEL_FLOW_DEVICE_QUERY_CONTEXT_HPP

#include <volk/volk.h>
#include <VoxFlow/Core/Utils/NonCopyable.hpp>

namespace VoxFlow
{
class LogicalDevice;
class Buffer;

enum class DeviceQueryMode : uint8_t
{
    PerformanceCounter = 0,
    Undefined = 1,
    Count = Undefined
};

class DeviceQueryContext : NonCopyable
{
 public:
    explicit DeviceQueryContext(LogicalDevice* logicalDevice);
    ~DeviceQueryContext() override;
    DeviceQueryContext(DeviceQueryContext&& context) noexcept;
    DeviceQueryContext& operator=(DeviceQueryContext&& context) noexcept;

    bool initialize(DeviceQueryMode queryMode);
    void resetQueryPool();

    void beginTimestampQuery(VkCommandBuffer vkCommandBuffer);
    void endTimestampQuery(VkCommandBuffer vkCommandBuffer);
    inline uint32_t getNumTimestamps() const
    {
        return _numTimestamps;
    }

    static constexpr uint32_t kMaxQueryCount = 1024;

 protected:
    void release();

 private:
    LogicalDevice* _logicalDevice = nullptr;
    DeviceQueryMode _queryMode = DeviceQueryMode::Undefined;

    VkQueryPool _vkQueryPool = VK_NULL_HANDLE;
    float _timestampPeriod = 0.0f;

    Buffer* _queryDataBuffer = nullptr;
    uint32_t _numTimestamps = 0;

    bool _mustResetQueryPool = false;
};
}  // namespace VoxFlow

#endif