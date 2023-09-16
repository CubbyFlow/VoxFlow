// Author : snowapril

#include <VoxFlow/Core/Devices/Instance.hpp>
#include <VoxFlow/Core/Devices/LogicalDevice.hpp>
#include <VoxFlow/Core/Devices/PhysicalDevice.hpp>
#include <VoxFlow/Core/Devices/SwapChain.hpp>
#include <VoxFlow/Core/Graphics/Commands/CommandJobSystem.hpp>
#include <VoxFlow/Core/Resources/Buffer.hpp>
#include <VoxFlow/Core/Graphics/RenderPass/RenderPassCollector.hpp>
#include <VoxFlow/Core/Graphics/Descriptors/DescriptorSetAllocatorPool.hpp>
#include <VoxFlow/Core/Resources/RenderResourceMemoryPool.hpp>
#include <VoxFlow/Core/Resources/ResourceUploadContext.hpp>
#include <VoxFlow/Core/Resources/Texture.hpp>
#include <VoxFlow/Core/Utils/DecisionMaker.hpp>
#include <VoxFlow/Core/Utils/Logger.hpp>
#include <optional>
#include <unordered_map>

namespace VoxFlow
{
LogicalDevice::LogicalDevice(const Context& ctx, PhysicalDevice* physicalDevice,
                             Instance* instance,
                             const LogicalDeviceType deviceType)
    : _physicalDevice(physicalDevice),
      _instance(instance),
      _deviceType(deviceType)
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
    void* pNextChain = nullptr;

    VkPhysicalDeviceVulkan13Features features13 = {};
    features13.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;
    features13.synchronization2 = VK_TRUE;
    features13.pNext = pNextChain;
    pNextChain = &features13;

    VkPhysicalDeviceVulkan12Features features12 = {};
    features12.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;
    features12.pNext = pNextChain;
    features12.timelineSemaphore = VK_TRUE;
    features12.descriptorIndexing = VK_TRUE;
    features12.descriptorBindingPartiallyBound = VK_TRUE;
    features12.descriptorBindingSampledImageUpdateAfterBind = VK_TRUE;
    features12.descriptorBindingUniformBufferUpdateAfterBind = VK_TRUE;
    features12.descriptorBindingStorageBufferUpdateAfterBind = VK_TRUE;
    pNextChain = &features12;

    [[maybe_unused]] const VkDeviceCreateInfo deviceInfo = {
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .pNext = pNextChain,
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

    VOX_ASSERT(
        _deviceDefaultResourceMemoryPool->initialize(),
        "Failed to initialize device-default render resource memory pool");

    _renderPassCollector = new RenderPassCollector(this);
    _descriptorSetAllocatorPool = new DescriptorSetAllocatorPool(this);

    initializeCommandStreams();
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

void LogicalDevice::initializeCommandStreams()
{
    _commandJobSystem = std::make_unique<CommandJobSystem>(this);

    _commandJobSystem->createCommandStream(
        CommandStreamKey{ ._cmdStreamName = MAIN_GRAPHICS_STREAM_NAME,
                          ._cmdStreamUsage = CommandStreamUsage::Graphics },
        getQueuePtr("MainGraphics"));

    _commandJobSystem->createCommandStream(
        CommandStreamKey{ ._cmdStreamName = ASYNC_COMPUTE_STREAM_NAME,
                          ._cmdStreamUsage = CommandStreamUsage::Compute },
        getQueuePtr("AsyncCompute"));

    _commandJobSystem->createCommandStream(
        CommandStreamKey{ ._cmdStreamName = ASYNC_UPLOAD_STREAM_NAME,
                          ._cmdStreamUsage = CommandStreamUsage::Transfer },
        getQueuePtr("AsyncUpload"));

    _commandJobSystem->createCommandStream(
        CommandStreamKey{ ._cmdStreamName = IMMEDIATE_UPLOAD_STREAM_NAME,
                          ._cmdStreamUsage = CommandStreamUsage::Transfer },
        getQueuePtr("ImmediateUpload"));
}

void LogicalDevice::releaseDedicatedResources()
{
    vkDeviceWaitIdle(_device);

    _swapChains.clear();

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