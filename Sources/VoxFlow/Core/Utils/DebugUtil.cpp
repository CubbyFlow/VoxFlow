// Author : snowapril

#include <spdlog/spdlog.h>
#include <VoxFlow/Core/Devices/LogicalDevice.hpp>
#include <VoxFlow/Core/Utils/DebugUtil.hpp>

namespace VoxFlow
{
VKAPI_ATTR VkBool32 VKAPI_CALL DebugUtil::DebugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT severity,
    VkDebugUtilsMessageTypeFlagsEXT flags,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* userData)
{
    (void)flags;
    (void)userData;

    switch (severity)
    {
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
            spdlog::debug("[Vulkan Validation] {}", pCallbackData->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
            spdlog::info("[Vulkan Validation] {}", pCallbackData->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
            spdlog::warn("[Vulkan Validation] {}", pCallbackData->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
            spdlog::error("[Vulkan Validation] {}", pCallbackData->pMessage);
            break;
        default:
            assert(severity <
                   VK_DEBUG_UTILS_MESSAGE_SEVERITY_FLAG_BITS_MAX_ENUM_EXT);
            break;
    }

    DebugUtil::DebugBreak();

    return VK_FALSE;
}

void DebugUtil::GlfwDebugCallback(int errorCode, const char* description)
{
    spdlog::warn("[GLFW Callback] {} ({})", description, errorCode);
}

void DebugUtil::DebugBreak()
{
#if defined(_WIN32)
    ::DebugBreak();
#endif
}

void DebugUtil::setObjectName(LogicalDevice* logicalDevice, uint64_t object, const char* name,
                              VkObjectType type)
{
    // static PFN_vkSetDebugUtilsObjectNameEXT setObjectName =
    const VkDebugUtilsObjectNameInfoEXT nameInfo = {
        .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT,
        .pNext = nullptr,
        .objectType = type,
        .objectHandle = object,
        .pObjectName = name
    };
    vkSetDebugUtilsObjectNameEXT(logicalDevice->get(), &nameInfo);
}

DeviceRemoveTracker* DeviceRemoveTracker::get()
{
    static DeviceRemoveTracker* sDeviceRemoveTrackerInst = nullptr;
    if (sDeviceRemoveTrackerInst == nullptr)
    {
        sDeviceRemoveTrackerInst = new DeviceRemoveTracker();
    }
    return sDeviceRemoveTrackerInst;
}

void DeviceRemoveTracker::addLogicalDeviceToTrack(LogicalDevice* logicalDevice)
{
    _logicalDevices.push_back(logicalDevice);
}

void DeviceRemoveTracker::onDeviceRemoved()
{
    VOX_ASSERT(false, "Not implemented yet");
}

std::string getVkResultString(VkResult vkResult)
{
#define VKSTR(str)   \
    case VK_##str:   \
        return #str; \
        break;

    switch (vkResult)
    {
        VKSTR(NOT_READY);
        VKSTR(TIMEOUT);
        VKSTR(EVENT_SET);
        VKSTR(EVENT_RESET);
        VKSTR(INCOMPLETE);
        VKSTR(ERROR_OUT_OF_HOST_MEMORY);
        VKSTR(ERROR_OUT_OF_DEVICE_MEMORY);
        VKSTR(ERROR_INITIALIZATION_FAILED);
        VKSTR(ERROR_DEVICE_LOST);
        VKSTR(ERROR_MEMORY_MAP_FAILED);
        VKSTR(ERROR_LAYER_NOT_PRESENT);
        VKSTR(ERROR_EXTENSION_NOT_PRESENT);
        VKSTR(ERROR_FEATURE_NOT_PRESENT);
        VKSTR(ERROR_INCOMPATIBLE_DRIVER);
        VKSTR(ERROR_TOO_MANY_OBJECTS);
        VKSTR(ERROR_FORMAT_NOT_SUPPORTED);
        VKSTR(ERROR_SURFACE_LOST_KHR);
        VKSTR(ERROR_NATIVE_WINDOW_IN_USE_KHR);
        VKSTR(SUBOPTIMAL_KHR);
        VKSTR(ERROR_OUT_OF_DATE_KHR);
        VKSTR(ERROR_INCOMPATIBLE_DISPLAY_KHR);
        VKSTR(ERROR_VALIDATION_FAILED_EXT);
        VKSTR(ERROR_INVALID_SHADER_NV);
        default:
            return "UNKNOWN_ERROR";
    }

#undef VKSTR
}

}  // namespace VoxFlow