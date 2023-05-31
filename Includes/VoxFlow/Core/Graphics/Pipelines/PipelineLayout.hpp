// Author : snowapril

#ifndef VOXEL_FLOW_PIPELINE_LAYOUT_HPP
#define VOXEL_FLOW_PIPELINE_LAYOUT_HPP

#include <volk/volk.h>
#include <VoxFlow/Core/Utils/NonCopyable.hpp>
#include <VoxFlow/Core/Graphics/Pipelines/ShaderLayoutBinding.hpp>
#include <memory>
#include <vector>
#include <array>

namespace VoxFlow
{
class LogicalDevice;
class DescriptorSetAllocator;

class PipelineLayout : NonCopyable
{
 public:
    explicit PipelineLayout(
        LogicalDevice* logicalDevice,
        std::vector<ShaderLayoutBinding>&& setLayoutBindings);
    ~PipelineLayout() override;
    PipelineLayout(PipelineLayout&& other) noexcept;
    PipelineLayout& operator=(PipelineLayout&& other) noexcept;

    [[nodiscard]] VkPipelineLayout get() const noexcept
    {
        return _vkPipelineLayout;
    }

    [[nodiscard]] DescriptorSetAllocator* getDescSetAllocator(const SetSlotCategory category) const
    {
        return _setAllocators[static_cast<uint32_t>(category)].get();
    }

 protected:
    void release();

 private:
    LogicalDevice* _logicalDevice = nullptr;
    VkPipelineLayout _vkPipelineLayout{ VK_NULL_HANDLE };
    std::array<std::shared_ptr<DescriptorSetAllocator>, MAX_NUM_SET_SLOTS> _setAllocators;
    std::vector<ShaderLayoutBinding> _setLayoutBindings;
};
}  // namespace VoxFlow

#endif