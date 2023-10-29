// Author : snowapril

#include <glslang_c_interface.h>
#include <volk/volk.h>
#include <VoxFlow/Core/Devices/Instance.hpp>
#include <VoxFlow/Core/Utils/DecisionMaker.hpp>
#include <VoxFlow/Core/Utils/Logger.hpp>

namespace VoxFlow
{
Instance::Instance(const Context& ctx)
{
    glslang_initialize_process();
    VK_ASSERT(volkInitialize());

    VkApplicationInfo appInfo = { .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
                                  .pNext = nullptr,
                                  .pApplicationName = ctx.appTitle.c_str(),
                                  .applicationVersion = VK_MAKE_VERSION(0, 0, 0),
                                  .pEngineName = ctx.appEngine.c_str(),
                                  .engineVersion = 0,
                                  .apiVersion = VK_MAKE_VERSION(ctx.majorVersion, ctx.minorVersion, 0) };

    uint32_t extensionCount;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
    std::vector<VkExtensionProperties> extensionProperties(extensionCount);
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensionProperties.data());

    std::vector<const char*> usedExtensions;
    std::vector<void*> featureStructs;
    VK_ASSERT(DecisionMaker::pickExtensions(usedExtensions, extensionProperties, ctx.instanceExtensions, featureStructs));

    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
    std::vector<VkLayerProperties> layerProperties(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, layerProperties.data());

    std::vector<const char*> usedLayers;
    VK_ASSERT(DecisionMaker::pickLayers(usedLayers, layerProperties, ctx.instanceLayers));

    [[maybe_unused]] VkInstanceCreateInfo instanceInfo = { .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
                                                           .pNext = nullptr,
                                                           .flags = 0,
                                                           .pApplicationInfo = &appInfo,
                                                           .enabledLayerCount = static_cast<uint32_t>(usedLayers.size()),
                                                           .ppEnabledLayerNames = usedLayers.data(),
                                                           .enabledExtensionCount = static_cast<uint32_t>(usedExtensions.size()),
                                                           .ppEnabledExtensionNames = usedExtensions.data() };

    // TODO(snowapril) : enable below features
    [[maybe_unused]] VkDebugUtilsMessengerCreateInfoEXT debugInfo;
    // [[maybe_unused]] VkValidationFeaturesEXT validationFeatures;
    // [[maybe_unused]] std::vector<VkValidationFeatureEnableEXT>
    //     enabledValidationFeatures;
    if (ctx.useValidationLayer)
    {
        debugInfo = {
            .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
            .pNext = nullptr,
            .flags = 0,
            .messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
            .messageType =
                VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
            .pfnUserCallback = DebugUtil::DebugCallback,
            .pUserData = nullptr,
        };
        instanceInfo.pNext = &debugInfo;
        //
        //     validationFeatures.sType = VK_STRUCTURE_TYPE_VALIDATION_FEATURES_EXT;
        //     enabledValidationFeatures.push_back(
        //         VK_VALIDATION_FEATURE_ENABLE_GPU_ASSISTED_EXT);
        //     enabledValidationFeatures.push_back(
        //         VK_VALIDATION_FEATURE_ENABLE_GPU_ASSISTED_RESERVE_BINDING_SLOT_EXT);
        //     enabledValidationFeatures.push_back(
        //         VK_VALIDATION_FEATURE_ENABLE_BEST_PRACTICES_EXT);
        //     enabledValidationFeatures.push_back(
        //         VK_VALIDATION_FEATURE_ENABLE_DEBUG_PRINTF_EXT);
        //     enabledValidationFeatures.push_back(
        //         VK_VALIDATION_FEATURE_ENABLE_SYNCHRONIZATION_VALIDATION_EXT);
        //     validationFeatures.enabledValidationFeatureCount = static_cast<uint32_t>(enabledValidationFeatures.size());
        //     validationFeatures.pEnabledValidationFeatures = enabledValidationFeatures.data();
        //     debugInfo.pNext = &validationFeatures;
    }

    VK_ASSERT(vkCreateInstance(&instanceInfo, nullptr, &_instance));
    volkLoadInstance(_instance);

    if (ctx.useValidationLayer)
    {
        VK_ASSERT(vkCreateDebugUtilsMessengerEXT(_instance, &debugInfo, nullptr, &_debugMessenger));
    }
}

Instance::~Instance()
{
    release();
    glslang_finalize_process();
}

Instance::Instance(Instance&& instance) noexcept : _instance(instance._instance)
{
    // Do nothing
}

Instance& Instance::operator=(Instance&& instance) noexcept
{
    if (this != &instance)
    {
        _instance = instance._instance;
    }
    return *this;
}

void Instance::release()
{
    if (_debugMessenger != VK_NULL_HANDLE)
    {
        vkDestroyDebugUtilsMessengerEXT(_instance, _debugMessenger, nullptr);
        _debugMessenger = VK_NULL_HANDLE;
    }

    if (_instance != VK_NULL_HANDLE)
    {
        vkDestroyInstance(_instance, nullptr);
        _instance = VK_NULL_HANDLE;
    }
}
}  // namespace VoxFlow
