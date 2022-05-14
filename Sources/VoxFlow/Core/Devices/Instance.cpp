// Author : snowapril

#include <VoxFlow/Core/Devices/Instance.hpp>
#include <VoxFlow/Core/Utils/DecisionMaker.hpp>
#include <VoxFlow/Core/Utils/Initializer.hpp>
#include <VoxFlow/Core/Utils/Logger.hpp>
#include <volk/volk.h>

#include <glslang_c_interface.h>

namespace VoxFlow
{
Instance::Instance(const Context& ctx)
{
    glslang_initialize_process();
    VK_ASSERT(volkInitialize());

    VkApplicationInfo appInfo = {
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pNext = nullptr,
        .pApplicationName = ctx.appTitle.c_str(),
        .applicationVersion = VK_MAKE_VERSION(0, 0, 0),
        .pEngineName = ctx.appEngine.c_str(),
        .engineVersion = 0,
        .apiVersion = VK_MAKE_VERSION(ctx.majorVersion, ctx.minorVersion, 0)
    };

    uint32_t extensionCount;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
    std::vector<VkExtensionProperties> extensionProperties(extensionCount);
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount,
                                           extensionProperties.data());

    std::vector<const char*> usedExtensions;
    std::vector<void*> featureStructs;
    VK_ASSERT(DecisionMaker::pickExtensions(usedExtensions, extensionProperties,
                                            ctx.instanceExtensions,
                                            featureStructs));

    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
    std::vector<VkLayerProperties> layerProperties(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, layerProperties.data());

    std::vector<const char*> usedLayers;
    VK_ASSERT(DecisionMaker::pickLayers(usedLayers, layerProperties,
                                        ctx.instanceLayers));

    [[maybe_unused]] VkInstanceCreateInfo instanceInfo = {
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .pApplicationInfo = &appInfo,
        .enabledLayerCount = static_cast<uint32_t>(usedLayers.size()),
        .ppEnabledLayerNames = usedLayers.data(),
        .enabledExtensionCount = static_cast<uint32_t>(usedExtensions.size()),
        .ppEnabledExtensionNames = usedExtensions.data()
    };

    [[maybe_unused]] VkDebugUtilsMessengerCreateInfoEXT debugInfo;
    if (ctx.useValidationLayer)
    {
        debugInfo = Initializer::MakeInfo<VkDebugUtilsMessengerCreateInfoEXT>();
        instanceInfo.pNext = &debugInfo;
    }

    VK_ASSERT(vkCreateInstance(&instanceInfo, nullptr, &_instance));
    volkLoadInstance(_instance);

    if (ctx.useValidationLayer)
    {
        VK_ASSERT(vkCreateDebugUtilsMessengerEXT(_instance, &debugInfo, nullptr,
                                                 &_debugMessenger));
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
