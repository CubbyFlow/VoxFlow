// Author : snowapril

#ifndef VOXEL_FLOW_PIPELINE_LAYOUT_HPP
#define VOXEL_FLOW_PIPELINE_LAYOUT_HPP

#include <volk/volk.h>
#include <VoxFlow/Core/Utils/NonCopyable.hpp>
#include <VoxFlow/Core/Graphics/Pipelines/PipelineLayoutDescriptor.hpp>
#include <VoxFlow/Core/Graphics/Descriptors/DescriptorSetConfig.hpp>
#include <memory>
#include <vector>
#include <array>

namespace VoxFlow
{
class LogicalDevice;
class DescriptorSetAllocator;
struct ShaderReflectionDataGroup;

class PipelineLayout : NonCopyable
{
 public:
    using ShaderVariableMap = std::unordered_map<std::string, DescriptorInfo>;

    explicit PipelineLayout(LogicalDevice* logicalDevice);
    ~PipelineLayout() override;
    PipelineLayout(PipelineLayout&& other) noexcept;
    PipelineLayout& operator=(PipelineLayout&& other) noexcept;

    /**
     * @return VkPipelineLayout created with descriptor set layouts which
     * contain shader module reflection infos
     */
    [[nodiscard]] VkPipelineLayout get() const noexcept
    {
        return _vkPipelineLayout;
    }

    /**
     * @param category descriptor set slot category to be queried
     * @return descriptor set allocator which match layout of shader resource
     * reflection contained in given category
     */
    [[nodiscard]] DescriptorSetAllocator* getDescSetAllocator(const SetSlotCategory category) const
    {
        return _setAllocators[static_cast<uint32_t>(category)].get();
    }

    /**
     * @return shader variables used from this pipeline layout
     */
    [[nodiscard]] const ShaderVariableMap& getShaderVariableMap()
        const
    {
        return _shaderVariableMap;
    }

    /**
     * @return pipeline layout descriptor combined with all shader modules
     * inserted
     */
    [[nodiscard]] const PipelineLayoutDescriptor& getPipelineLayoutDescriptor()
        const
    {
        return _combinedPipelineLayoutDesc;
    }

 public:
    /**
     * Combine given shader layout bindings into descriptor set layouts with conflict resolved.
     * The shader resource binding declared in different shader module can be resolved by this process.
     * Get descriptor set allocator from the pool which match to each combined descriptor set layout
     * 
     * @param setLayoutBindings shader layout binding which match to reflections of each shader module
     * @return whether pipeline layout creation is success or not
     */
    bool initialize(const std::vector<const ShaderReflectionDataGroup*>&
                        combinedReflectionDataGroups);

 protected:
    /**
     * Release created pipeline layout and descriptor set allocators
     */
    void release();

 private:
    LogicalDevice* _logicalDevice = nullptr;
    VkPipelineLayout _vkPipelineLayout{ VK_NULL_HANDLE };
    std::array<std::shared_ptr<DescriptorSetAllocator>, MAX_NUM_SET_SLOTS> _setAllocators;
    PipelineLayoutDescriptor _combinedPipelineLayoutDesc;
    ShaderVariableMap _shaderVariableMap;
};
}  // namespace VoxFlow

#endif