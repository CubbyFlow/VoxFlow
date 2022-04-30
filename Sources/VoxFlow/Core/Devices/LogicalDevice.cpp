// Author : snowapril

#include <VoxFlow/Core/Devices/LogicalDevice.hpp>
#include <VoxFlow/Core/Utils/DecisionMaker.hpp>
#include <VoxFlow/Core/Utils/Logger.hpp>
#include <optional>

namespace VoxFlow
{
LogicalDevice::LogicalDevice(const Context& ctx,
                             const PhysicalDevice& physicalDevice)
{
    const std::vector<VkExtensionProperties> extensionProperties =
        physicalDevice.getPossibleExtensions();
    std::vector<const char*> usedExtensions;
    std::vector<void*> featureStructs;
    VK_ASSERT(DecisionMaker::pickExtensions(usedExtensions, extensionProperties,
                                            ctx.deviceExtensions,
                                            featureStructs) == VK_SUCCESS);

    const auto queueFamilies = physicalDevice.getQueueFamilyProperties();

    // TODO: sort queue family indices according to requested priorities
    std::vector<uint32_t> queueFamilyIndices;
    std::vector<VkDeviceQueueCreateInfo> queueInfos;
    queueFamilyIndices.reserve(ctx.requiredQueues.size());
    queueInfos.reserve(ctx.requiredQueues.size());

    // TODO: move picking required queues to DecisionMaker
    for (const auto& requiredQueue : ctx.requiredQueues)
    {
        uint32_t index = 0;
        std::optional<uint32_t> familyIndex = std::nullopt;

        for (const auto& queueFamily : queueFamilies)
        {
            if ((queueFamily.queueCount >= requiredQueue.queueCount) &&
                (queueFamily.queueFlags && requiredQueue.flag))
            {
                familyIndex = index;
            }

            if (familyIndex.has_value() &&
                std::ranges::find(
                    queueFamilyIndices.begin(), queueFamilyIndices.end(),
                    familyIndex.value()) == queueFamilyIndices.end())
            {
                break;
            }
            index++;
        }

        if (familyIndex.has_value() == false)
        {
            spdlog::error("Failed to find required queue [{}] in this device.",
                          requiredQueue.queueName);
            std::abort();
        }
        else
        {
            queueFamilyIndices.push_back(familyIndex.value());
            queueInfos.push_back(
                { .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
                  .pNext = nullptr,
                  .flags = 0,
                  .queueFamilyIndex = familyIndex.value(),
                  .queueCount = requiredQueue.queueCount,
                  .pQueuePriorities = &requiredQueue.priority });
        }
    }

    [[maybe_unused]] const VkDeviceCreateInfo deviceInfo = {
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .queueCreateInfoCount = static_cast<uint32_t>(queueInfos.size()),
        .pQueueCreateInfos = queueInfos.data(),
        .enabledLayerCount = 0u,
        .ppEnabledLayerNames = nullptr,
        .enabledExtensionCount = static_cast<uint32_t>(usedExtensions.size()),
        .ppEnabledExtensionNames = usedExtensions.data(),
        .pEnabledFeatures = nullptr
    };

    VK_ASSERT(vkCreateDevice(physicalDevice.get(), &deviceInfo, nullptr,
                             &_device) == VK_SUCCESS);

    for (size_t i = 0; i < ctx.requiredQueues.size(); ++i)
    {
        VkQueue queueHandle;
        vkGetDeviceQueue(_device, queueFamilyIndices[i], 0, &queueHandle);
        _queueMap.emplace(
            ctx.requiredQueues[i].queueName,
            std::make_shared<Queue>(queueHandle, queueFamilyIndices[i]));
    }
}

LogicalDevice::~LogicalDevice()
{
    release();
}

LogicalDevice::LogicalDevice(LogicalDevice&& other) noexcept
    : _device(std::move(other._device)), _queueMap(std::move(other._queueMap))
{
    // Do nothing
}

LogicalDevice& LogicalDevice::operator=(LogicalDevice&& other) noexcept
{
    if (this != &other)
    {
        _device = std::move(other._device);
        _queueMap = std::move(other._queueMap);
    }
    return *this;
}

std::weak_ptr<Queue> LogicalDevice::getQueuePtr(const std::string& queueName)
{
    auto iter = _queueMap.find(queueName);
    assert(iter != _queueMap.end());
    return iter->second;
}

void LogicalDevice::release() const
{
    vkDestroyDevice(_device, nullptr);
}
}  // namespace VoxFlow