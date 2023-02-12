// Author : snowapril

#ifndef VOXEL_FLOW_TEXTURE_HPP
#define VOXEL_FLOW_TEXTURE_HPP

#include <volk/volk.h>
#include <vma/include/vk_mem_alloc.h>
#include <VoxFlow/Core/Utils/NonCopyable.hpp>
#include <VoxFlow/Core/Utils/RendererCommon.hpp>
#include <string>

namespace VoxFlow
{

class LogicalDevice;
class RenderResourceMemoryPool;

class Texture : private NonCopyable
{
 public:
    explicit Texture(std::string&& debugName, LogicalDevice* logicalDevice,
                     RenderResourceMemoryPool* renderResourceMemoryPool);
    ~Texture();

 public:
    [[nodiscard]] inline VkImage get() const
    {
        return _vkImage;
    }

    bool initialize(TextureInfo textureInfo);
    void release();

 protected:
 private:
    LogicalDevice* _logicalDevice = nullptr;
    RenderResourceMemoryPool* _renderResourceMemoryPool = nullptr;
    VkImage _vkImage = VK_NULL_HANDLE;
    VmaAllocation _textureAllocation = nullptr;
    TextureInfo _textureInfo;
    std::string _debugName;
};
}  // namespace VoxFlow

#endif