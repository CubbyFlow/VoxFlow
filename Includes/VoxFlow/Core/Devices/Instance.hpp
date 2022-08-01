// Author : snowapril

#ifndef VOXEL_FLOW_INSTANCE_HPP
#define VOXEL_FLOW_INSTANCE_HPP

#include <volk/volk.h>
#include <VoxFlow/Core/Devices/Context.hpp>
#include <VoxFlow/Core/Utils/NonCopyable.hpp>

namespace VoxFlow
{
class Instance : NonCopyable
{
 public:
    explicit Instance(const Context& ctx);
    ~Instance() override;
    Instance(Instance&& instance) noexcept;
    Instance& operator=(Instance&& instance) noexcept;

    [[nodiscard]] VkInstance get() const noexcept
    {
        return _instance;
    }

 protected:
    void release();

 private:
    VkInstance _instance{ VK_NULL_HANDLE };
    VkDebugUtilsMessengerEXT _debugMessenger{ VK_NULL_HANDLE };
};
}  // namespace VoxFlow

#endif