// Author : snowapril

#ifndef VOXEL_FLOW_DEBUG_UTIL_HPP
#define VOXEL_FLOW_DEBUG_UTIL_HPP

#include <volk/volk.h>
#include <memory>

namespace VoxFlow
{
class LogicalDevice;

class DebugUtil
{
 public:
    static VKAPI_ATTR VkBool32 VKAPI_CALL
    DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT severity,
                  VkDebugUtilsMessageTypeFlagsEXT flags,
                  const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                  void* userData);
    static void GlfwDebugCallback(int errorCode, const char* description);
    static void DebugBreak();

 public:
    struct ScopedCmdLabel
    {
        ScopedCmdLabel(VkCommandBuffer cmdBuffer, const char* label)
            : _cmdBuffer(cmdBuffer)
        {
            const VkDebugUtilsLabelEXT labelInfo = {
                .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_LABEL_EXT,
                .pNext = nullptr,
                .pLabelName = label,
                .color = { 1.0f, 1.0f, 1.0f, 1.0f }
            };
            vkCmdBeginDebugUtilsLabelEXT(_cmdBuffer, &labelInfo);
        }
        ~ScopedCmdLabel()
        {
            vkCmdEndDebugUtilsLabelEXT(_cmdBuffer);
        }
        ScopedCmdLabel(const ScopedCmdLabel&) = default;
        ScopedCmdLabel& operator=(const ScopedCmdLabel&) = default;
        ScopedCmdLabel(ScopedCmdLabel&&) = default;
        ScopedCmdLabel& operator=(ScopedCmdLabel&&) = default;

        void insertLabel(const char* label) const
        {
            const VkDebugUtilsLabelEXT labelInfo = {
                .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_LABEL_EXT,
                .pNext = nullptr,
                .pLabelName = label,
                .color = { 1.0f, 1.0f, 1.0f, 1.0f }
            };
            vkCmdInsertDebugUtilsLabelEXT(_cmdBuffer, &labelInfo);
        }

     private:
        VkCommandBuffer _cmdBuffer{ VK_NULL_HANDLE };
    };

    static ScopedCmdLabel scopeLabel(VkCommandBuffer cmdBuffer,
                                     const char* label)
    {
        return { cmdBuffer, label };
    }

#if defined(VK_DEBUG_NAME_ENABLED)
    static void setObjectName(LogicalDevice* logicalDevice,
                              VkCommandBuffer commandBuffer, const char* name)
    {
        setObjectName(logicalDevice, reinterpret_cast<uint64_t>(commandBuffer),
                      name, VK_OBJECT_TYPE_COMMAND_BUFFER);
    }
    static void setObjectName(LogicalDevice* logicalDevice,
                              VkCommandPool commandPool, const char* name)
    {
        setObjectName(logicalDevice, reinterpret_cast<uint64_t>(commandPool),
                      name, VK_OBJECT_TYPE_COMMAND_POOL);
    }
    static void setObjectName(LogicalDevice* logicalDevice, VkBuffer buffer,
                              const char* name)
    {
        setObjectName(logicalDevice, reinterpret_cast<uint64_t>(buffer), name,
                      VK_OBJECT_TYPE_BUFFER);
    }
    static void setObjectName(LogicalDevice* logicalDevice, VkImage image,
                              const char* name)
    {
        setObjectName(logicalDevice, reinterpret_cast<uint64_t>(image), name,
                      VK_OBJECT_TYPE_IMAGE);
    }
    static void setObjectName(LogicalDevice* logicalDevice,
                              VkImageView imageView, const char* name)
    {
        setObjectName(logicalDevice, reinterpret_cast<uint64_t>(imageView),
                      name, VK_OBJECT_TYPE_IMAGE_VIEW);
    }
    static void setObjectName(LogicalDevice* logicalDevice, VkSampler sampler,
                              const char* name)
    {
        setObjectName(logicalDevice, reinterpret_cast<uint64_t>(sampler), name,
                      VK_OBJECT_TYPE_SAMPLER);
    }
    static void setObjectName(LogicalDevice* logicalDevice, VkPipeline pipeline,
                              const char* name)
    {
        setObjectName(logicalDevice, reinterpret_cast<uint64_t>(pipeline), name,
                      VK_OBJECT_TYPE_PIPELINE);
    }
    static void setObjectName(LogicalDevice* logicalDevice,
                              VkRenderPass renderPass, const char* name)
    {
        setObjectName(logicalDevice, reinterpret_cast<uint64_t>(renderPass),
                      name, VK_OBJECT_TYPE_RENDER_PASS);
    }
    static void setObjectName(LogicalDevice* logicalDevice, VkFence fence,
                              const char* name)
    {
        setObjectName(logicalDevice, reinterpret_cast<uint64_t>(fence), name,
                      VK_OBJECT_TYPE_FENCE);
    }
    static void setObjectName(LogicalDevice* logicalDevice, VkFramebuffer frameBuffer,
                              const char* name)
    {
        setObjectName(logicalDevice, reinterpret_cast<uint64_t>(frameBuffer),
                      name, VK_OBJECT_TYPE_FRAMEBUFFER);
    }
    static void setObjectName(LogicalDevice* logicalDevice,
                              VkSemaphore semaphore, const char* name)
    {
        setObjectName(logicalDevice, reinterpret_cast<uint64_t>(semaphore),
                      name, VK_OBJECT_TYPE_SEMAPHORE);
    }
    static void setObjectName(LogicalDevice* logicalDevice,
                              VkDescriptorSet descSet, const char* name)
    {
        setObjectName(logicalDevice, reinterpret_cast<uint64_t>(descSet), name,
                      VK_OBJECT_TYPE_DESCRIPTOR_SET);
    }
    static void setObjectName(LogicalDevice* logicalDevice,
                              VkBufferView bufferView, const char* name)
    {
        setObjectName(logicalDevice, reinterpret_cast<uint64_t>(bufferView),
                      name, VK_OBJECT_TYPE_BUFFER_VIEW);
    }
    static void setObjectName(LogicalDevice* logicalDevice, VkQueue queue,
                              const char* name)
    {
        setObjectName(logicalDevice, reinterpret_cast<uint64_t>(queue), name,
                      VK_OBJECT_TYPE_QUEUE);
    }
    static void setObjectName(LogicalDevice* logicalDevice,
                              VkSwapchainKHR swapChain, const char* name)
    {
        setObjectName(logicalDevice, reinterpret_cast<uint64_t>(swapChain),
                      name, VK_OBJECT_TYPE_SWAPCHAIN_KHR);
    }

 private:
    static void setObjectName(LogicalDevice* logicalDevice, uint64_t object,
                              const char* name, VkObjectType type);
#endif

    static uint64_t NumValidationErrorDetected;
};

class DeviceRemoveTracker
{
 public:
    DeviceRemoveTracker() = default;
    ~DeviceRemoveTracker() = default;

 public:
    static DeviceRemoveTracker* get();

    void addLogicalDeviceToTrack(LogicalDevice* logicalDevice);
    void onDeviceRemoved();

 private:
    std::vector<LogicalDevice*> _logicalDevices;
};

const char* getVkResultString(VkResult vkResult);

}  // namespace VoxFlow

#endif