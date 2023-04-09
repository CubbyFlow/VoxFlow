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
    DebugUtil() = default;
    explicit DebugUtil(const std::shared_ptr<LogicalDevice>& device)
        : _device(device){};

 public:
    static VKAPI_ATTR VkBool32 VKAPI_CALL
    DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT severity,
                  VkDebugUtilsMessageTypeFlagsEXT flags,
                  const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                  void* userData);
    static void GlfwDebugCallback(int errorCode, const char* description);

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
    void setObjectName(VkBuffer buffer, const char* name) const
    {
        setObjectName(reinterpret_cast<uint64_t>(buffer), name,
                      VK_OBJECT_TYPE_BUFFER);
    }
    void setObjectName(VkImage image, const char* name) const
    {
        setObjectName(reinterpret_cast<uint64_t>(image), name,
                      VK_OBJECT_TYPE_IMAGE);
    }
    void setObjectName(VkImageView imageView, const char* name) const
    {
        setObjectName(reinterpret_cast<uint64_t>(imageView), name,
                      VK_OBJECT_TYPE_IMAGE_VIEW);
    }
    void setObjectName(VkSampler sampler, const char* name) const
    {
        setObjectName(reinterpret_cast<uint64_t>(sampler), name,
                      VK_OBJECT_TYPE_SAMPLER);
    }
    void setObjectName(VkPipeline pipeline, const char* name) const
    {
        setObjectName(reinterpret_cast<uint64_t>(pipeline), name,
                      VK_OBJECT_TYPE_PIPELINE);
    }
    void setObjectName(VkRenderPass renderPass, const char* name) const
    {
        setObjectName(reinterpret_cast<uint64_t>(renderPass), name,
                      VK_OBJECT_TYPE_RENDER_PASS);
    }
    void setObjectName(VkFence fence, const char* name) const
    {
        setObjectName(reinterpret_cast<uint64_t>(fence), name,
                      VK_OBJECT_TYPE_FENCE);
    }
    void setObjectName(VkSemaphore semaphore, const char* name) const
    {
        setObjectName(reinterpret_cast<uint64_t>(semaphore), name,
                      VK_OBJECT_TYPE_SEMAPHORE);
    }
    void setObjectName(VkDescriptorSet descSet, const char* name) const
    {
        setObjectName(reinterpret_cast<uint64_t>(descSet), name,
                      VK_OBJECT_TYPE_DESCRIPTOR_SET);
    }
    void setObjectName(VkBufferView bufferView, const char* name) const
    {
        setObjectName(reinterpret_cast<uint64_t>(bufferView), name,
                      VK_OBJECT_TYPE_BUFFER_VIEW);
    }

 private:
    void setObjectName(uint64_t object, const char* name,
                       VkObjectType type) const;

 private:
    std::shared_ptr<LogicalDevice> _device{ nullptr };
};
}  // namespace VoxFlow

#endif