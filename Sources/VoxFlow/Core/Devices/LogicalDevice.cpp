// Author : snowapril

#include <VoxFlow/Core/Devices/LogicalDevice.hpp>
#include <VoxFlow/Core/Utils/Logger.hpp>

#include "VoxFlow/Core/Utils/DecisionMaker.hpp"

namespace VoxFlow
{
LogicalDevice::LogicalDevice(const Context& ctx,
                             const PhysicalDevice& physicalDevice)
{
    std::vector<VkExtensionProperties> extensionProperties =
        physicalDevice.getPossibleExtensions();
    std::vector<const char*> usedExtensions;
    std::vector<void*> featureStructs;
    VK_ASSERT(DecisionMaker::pickExtensions(usedExtensions, extensionProperties,
                                            ctx.deviceExtensions,
                                            featureStructs));

    // TODO: queue create infos need to be filled
    VkDeviceCreateInfo deviceInfo = {
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .queueCreateInfoCount =
            static_cast<uint32_t>(ctx.requiredQueues.size()),
        .pQueueCreateInfos = nullptr,
        .enabledLayerCount = 0u,
        .ppEnabledLayerNames = nullptr,
        .enabledExtensionCount = static_cast<uint32_t>(usedExtensions.size()),
        .ppEnabledExtensionNames = usedExtensions.data(),
        .pEnabledFeatures = nullptr
    };
}

LogicalDevice::~LogicalDevice()
{
    release();
}

void LogicalDevice::release() const
{
    vkDestroyDevice(_device, nullptr);
}
}  // namespace VoxFlow