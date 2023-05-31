// Author : snowapril

#ifndef VOXEL_FLOW_RENDER_DEVICE_HPP
#define VOXEL_FLOW_RENDER_DEVICE_HPP

#include <VoxFlow/Core/Devices/Context.hpp>
#include <VoxFlow/Core/Utils/Logger.hpp>
#include <VoxFlow/Core/Utils/NonCopyable.hpp>
#include <vector>

namespace VoxFlow
{
class Instance;
class PhysicalDevice;
class LogicalDevice;

class RenderDevice : private NonCopyable
{
 public:
    explicit RenderDevice(Context deviceSetupCtx);
    ~RenderDevice();

 public:
    [[nodiscard]] inline Instance* getInstance() const
    {
        return _instance;
    }
    [[nodiscard]] inline PhysicalDevice* getPhysicalDevice() const
    {
        return _physicalDevice;
    }
    [[nodiscard]] inline LogicalDevice* getLogicalDevice(
        const uint32_t deviceIndex) const
    {
        VOX_ASSERT(deviceIndex < _logicalDevices.size(),
                   "Given Index({}), Num LogicalDevices({})", deviceIndex,
                   _logicalDevices.size());
        return _logicalDevices.at(deviceIndex).get();
    }

 protected:
 private:
    Instance* _instance = nullptr;
    PhysicalDevice* _physicalDevice = nullptr;
    std::vector<std::unique_ptr<LogicalDevice>> _logicalDevices;
    Context* _deviceSetupCtx = nullptr;
    
};
}  // namespace VoxFlow

#endif