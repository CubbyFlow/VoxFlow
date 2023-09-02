// Author : snowapril

#ifndef VOXEL_FLOW_SAMPLER_HPP
#define VOXEL_FLOW_SAMPLER_HPP

#include <volk/volk.h>
#include <vma/include/vk_mem_alloc.h>
#include <VoxFlow/Core/Resources/RenderResource.hpp>

namespace VoxFlow
{
class LogicalDevice;
class RenderResourceMemoryPool;

class Sampler final : public RenderResource
{
 public:
    explicit Sampler(std::string_view&& debugName, LogicalDevice* logicalDevice);
    ~Sampler() override;

 public:
    [[nodiscard]] inline VkSampler get() const
    {
        return _vkSampler;
    }

    [[nodiscard]] inline RenderResourceType getResourceType() const override
    {
        return RenderResourceType::Sampler;
    }

    bool initialize();

    // Release buffer object to fence resource manager
    void release();

 protected:
 private:
    VkSampler _vkSampler = VK_NULL_HANDLE;
};
}  // namespace VoxFlow

#endif