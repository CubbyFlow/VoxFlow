// Author : snowapril

#include <VoxFlow/Core/Devices/Instance.hpp>
#include <VoxFlow/Core/Devices/LogicalDevice.hpp>
#include <VoxFlow/Core/Devices/PhysicalDevice.hpp>
#include <VoxFlow/Core/Devices/SwapChain.hpp>
#include <VoxFlow/Core/Graphics/RenderPass/RenderPassCollector.hpp>
#include <VoxFlow/Core/Resources/Buffer.hpp>
#include <VoxFlow/Core/Graphics/Descriptors/DescriptorSetAllocatorPool.hpp>
#include <VoxFlow/Core/Resources/RenderResourceMemoryPool.hpp>
#include <VoxFlow/Core/Resources/Texture.hpp>
#include <VoxFlow/Core/Utils/DecisionMaker.hpp>
#include <VoxFlow/Core/Utils/Logger.hpp>
#include <optional>
#include <unordered_map>

namespace VoxFlow
{
LogicalDevice::LogicalDevice(const Context& ctx, PhysicalDevice* physicalDevice,
                             Instance* instance)
    : _physicalDevice(physicalDevice), _instance(instance)
{
    const std::vector<VkLayerProperties> layerProperties =
        physicalDevice->getPossibleLayers();

    std::vector<const char*> usedLayers;
    // As instance layers are same with device layers, we can use it again
    VK_ASSERT(DecisionMaker::pickLayers(usedLayers, layerProperties,
                                        ctx.instanceLayers));

    const std::vector<VkExtensionProperties> extensionProperties =
        physicalDevice->getPossibleExtensions();

    std::vector<const char*> usedExtensions;
    std::vector<void*> featureStructs;
    VK_ASSERT(DecisionMaker::pickExtensions(usedExtensions, extensionProperties,
                                            ctx.deviceExtensions,
                                            featureStructs));

    const auto queueFamilies = physicalDevice->getQueueFamilyProperties();

    // TODO: sort queue family indices according to requested priorities
    std::vector<VkQueueFlags> requiredQueueFlags;
    std::vector<uint32_t> requiredQueueFamilyIndices;
    std::vector<VkDeviceQueueCreateInfo> queueInfos;
    requiredQueueFamilyIndices.reserve(ctx.requiredQueues.size());
    queueInfos.reserve(ctx.requiredQueues.size());

    // TODO: move picking required queues to DecisionMaker
    for (const auto& requiredQueue : ctx.requiredQueues)
    {
        uint32_t index = 0;
        std::optional<uint32_t> familyIndex = std::nullopt;
        std::optional<VkQueueFlags> queueFlags = std::nullopt;

        for (const auto& queueFamily : queueFamilies)
        {
            if ((queueFamily.queueCount >= requiredQueue.queueCount) &&
                (queueFamily.queueFlags && requiredQueue.flag))
            {
                familyIndex = index;
                queueFlags = queueFamily.queueFlags;
            }

            if (familyIndex.has_value() &&
                std::find(requiredQueueFamilyIndices.begin(),
                          requiredQueueFamilyIndices.end(),
                          familyIndex.value()) ==
                    requiredQueueFamilyIndices.end())
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
            requiredQueueFlags.push_back(queueFlags.value());
            requiredQueueFamilyIndices.push_back(familyIndex.value());
            queueInfos.push_back(
                { .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
                  .pNext = nullptr,
                  .flags = 0,
                  .queueFamilyIndex = familyIndex.value(),
                  .queueCount = requiredQueue.queueCount,
                  .pQueuePriorities = &requiredQueue.priority });
        }
    }

    // TODO(snowapril) : expose feature control
    VkPhysicalDeviceVulkan12Features features12 = {};
    features12.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;
    features12.pNext = VK_NULL_HANDLE;
    features12.timelineSemaphore = VK_TRUE;

    [[maybe_unused]] const VkDeviceCreateInfo deviceInfo = {
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .pNext = &features12,
        .flags = 0,
        .queueCreateInfoCount = static_cast<uint32_t>(queueInfos.size()),
        .pQueueCreateInfos = queueInfos.data(),
        .enabledLayerCount = static_cast<uint32_t>(usedLayers.size()),
        .ppEnabledLayerNames = usedLayers.data(),
        .enabledExtensionCount = static_cast<uint32_t>(usedExtensions.size()),
        .ppEnabledExtensionNames = usedExtensions.data(),
        .pEnabledFeatures = nullptr
    };

    VK_ASSERT(
        vkCreateDevice(physicalDevice->get(), &deviceInfo, nullptr, &_device));

    std::unordered_map<uint32_t, uint32_t> queueIndicesPerFamily;

    for (size_t i = 0; i < ctx.requiredQueues.size(); ++i)
    {
        VkQueue queueHandle;
        vkGetDeviceQueue(_device, requiredQueueFamilyIndices[i], 0,
                         &queueHandle);

        VOX_ASSERT(queueHandle != VK_NULL_HANDLE, "Failed to get device queue");
        if (queueHandle != VK_NULL_HANDLE)
        {
            std::unordered_map<uint32_t, uint32_t>::iterator findIt =
                queueIndicesPerFamily.find(requiredQueueFamilyIndices[i]);

            uint32_t queueIndex = 0U;
            if (findIt != queueIndicesPerFamily.end())
            {
                queueIndex = ++(findIt->second);
            }
            else
            {
                queueIndicesPerFamily.insert(
                    std::make_pair(requiredQueueFamilyIndices[i], queueIndex));
            }

            Queue* queue = new Queue(ctx.requiredQueues[i].queueName, this,
                                     requiredQueueFlags[i], queueHandle,
                                     requiredQueueFamilyIndices[i], queueIndex);

            VOX_ASSERT(queue != nullptr, "Failed to allocate queue");

            _queueMap.emplace(ctx.requiredQueues[i].queueName, queue);

            if (ctx.requiredQueues[i].isMainQueue)
            {
                _mainQueue = queue;
            }
        }
    }

    // Load device-related vulkan entrypoints (all global functions)
    volkLoadDevice(_device);

    DeviceRemoveTracker::get()->addLogicalDeviceToTrack(this);
    _deviceDefaultResourceMemoryPool =
        new RenderResourceMemoryPool(this, _physicalDevice, _instance); 
    _renderPassCollector = new RenderPassCollector(this);
    _descriptorSetAllocatorPool = new DescriptorSetAllocatorPool(this);
}

LogicalDevice::~LogicalDevice()
{
    releaseDedicatedResources();
    release();
}

LogicalDevice::LogicalDevice(LogicalDevice&& other) noexcept
    : _device(other._device), _queueMap(std::move(other._queueMap))
{
    // Do nothing
}

LogicalDevice& LogicalDevice::operator=(LogicalDevice&& other) noexcept
{
    if (this != &other)
    {
        _device = other._device;
        _queueMap = std::move(other._queueMap);
    }
    return *this;
}

Queue* LogicalDevice::getQueuePtr(const std::string& queueName)
{
    const auto iter = _queueMap.find(queueName);
    assert(iter != _queueMap.end());
    return iter->second;
}

std::shared_ptr<SwapChain> LogicalDevice::addSwapChain(
    const char* title, const glm::ivec2 resolution)
{
    std::shared_ptr<SwapChain> swapChain = std::make_shared<SwapChain>(
        _instance, _physicalDevice, this, _mainQueue, title, resolution);

    VOX_ASSERT(swapChain->create(), "Failed to create swapchain (name : {})",
               title);

    _swapChains.push_back(swapChain);

    return swapChain;
}

void LogicalDevice::releaseDedicatedResources()
{
    vkDeviceWaitIdle(_device);

    if (_deviceDefaultResourceMemoryPool != nullptr)
    {
        delete _deviceDefaultResourceMemoryPool;
    }

    if (_renderPassCollector != nullptr)
    {
        delete _renderPassCollector;
    }

    if (_descriptorSetAllocatorPool != nullptr)
    {
        delete _descriptorSetAllocatorPool;
    }

    _swapChains.clear();

    std::for_each(
        _queueMap.begin(), _queueMap.end(),
        [](std::unordered_map<std::string, Queue*>::value_type& queue) {
            if (queue.second != nullptr)
            {
                delete queue.second;
            }
        });
}

void LogicalDevice::release()
{
    if (_device != VK_NULL_HANDLE)
    {
        vkDestroyDevice(_device, nullptr);
        _device = VK_NULL_HANDLE;
    }
}
}  // namespace VoxFlow