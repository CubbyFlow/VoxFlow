// Author : snowapril

#ifndef VOXEL_FLOW_DEVICE_INPUT_REGISTRATOR_HPP
#define VOXEL_FLOW_DEVICE_INPUT_REGISTRATOR_HPP

#include <VoxFlow/Core/Utils/NonCopyable.hpp>
#include <bitset>
#include <functional>

struct GLFWwindow;

namespace VoxFlow
{
enum class DeviceInputType : uint8_t
{
    Key = 0,
    Motion = 1,
    Undefined = 2,
    Count = Undefined,
};

enum class DeviceKeyInputType : uint32_t
{
    Escape = 0,
    LeftClick = 1,
    RightClick = 2,
    KeyC = 3,
    Undefined = 4,
    Count = Undefined
};

using DeviceKeyInputCallback = std::function<void(DeviceKeyInputType key, bool isReleased)>;

class DeviceInputSubscriber : NonCopyable
{
 public:
    DeviceInputSubscriber();
    ~DeviceInputSubscriber();

    static constexpr uint32_t sMaxAvailableNumWindows = 32U;

    template <typename InputType>
    using DeviceInputRegistration = std::pair<std::bitset<sMaxAvailableNumWindows>, InputType>;

 public:
    void addObserveTargetWindow(GLFWwindow* window);
    void removeObserveTargetWindow(GLFWwindow* window);

    void registerDeviceKeyCallback(const uint32_t targetWindowBits, DeviceKeyInputCallback&& callback);

 public:
    void broadcastKeyInput(uint32_t windowIndex, DeviceKeyInputType key, bool isReleased);

 private:
    std::vector<DeviceInputRegistration<DeviceKeyInputCallback>> _deviceKeyInputCallbacks;
};
}  // namespace VoxFlow

#endif