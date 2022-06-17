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
    return VK_FALSE;
}

void DebugUtil::GlfwDebugCallback(int errorCode, const char* description)
{
    spdlog::warn("[GLFW Callback] {} ({})", description, errorCode);
}

void DebugUtil::setObjectName(uint64_t object, const char* name,
                              VkObjectType type) const
{
    // static PFN_vkSetDebugUtilsObjectNameEXT setObjectName =
    const VkDebugUtilsObjectNameInfoEXT nameInfo = {
        .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT,
        .pNext = nullptr,
        .objectType = type,
        .objectHandle = object,
        .pObjectName = name
    };
    vkSetDebugUtilsObjectNameEXT(_device->get(), &nameInfo);
}
}  // namespace VoxFlow