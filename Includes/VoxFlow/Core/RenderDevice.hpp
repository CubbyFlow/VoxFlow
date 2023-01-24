// Author : snowapril

#ifndef VOXEL_FLOW_RENDER_DEVICE_HPP
#define VOXEL_FLOW_RENDER_DEVICE_HPP

#include <VoxFlow/Core/Utils/DebugUtils>

namespace VoxFlow
{
class RenderDevice : private NonCopyable
{
 public
    RenderDevice(Context deviceSetupCtx);
    ~RenderDevice();

 public:
    inline Instance* getInstance() const
    {
        return _instance;
    }
    inline PhysicalDevice* getPhysicalDevice() const
    {
        return _physicalDevice;
    }
    inline PhysicalDevice* getLogicalDevice(const uint32_t deviceIndex) const
    {
        VOX_ASSERT(deviceIndex < _logicalDevices.size(),
                   "Given Index(%u), Num LogicalDevices(%u)", deviceIndex,
        return _logicalDevices.at(deviceIndex);
    }

 protected:
 private:
    Instance* _instance = nullptr;
    PhysicalDevice* _physicalDevice = nullptr;
    std::vector<LogicalDevice*> _logicalDevices;

    Context _deviceSetupCtx = {};
};
}  // namespace VoxFlow

#endif