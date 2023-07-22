// Author : snowapril

#ifndef VOXEL_FLOW_DESCRIPTOR_SET_HPP
#define VOXEL_FLOW_DESCRIPTOR_SET_HPP

#include <VoxFlow/Core/Utils/RendererCommon.hpp>
#include <memory>
#include <vector>

namespace VoxFlow
{

enum class DescriptorCategory : uint8_t
{
    CombinedImage = 0,
    UniformBuffer = 1,
    StorageBuffer = 2,
    Undefined = 3,
    Count = Undefined
};

struct DescriptorInfo
{
    DescriptorCategory _category = DescriptorCategory::Undefined;
    uint32_t _arraySize = 0;
    uint32_t _binding = 0;

    inline bool isValid() const
    {
        return (_category != DescriptorCategory::Undefined);
    }

    inline bool operator==(const DescriptorInfo& rhs) const
    {
        return (_category == rhs._category) && (_arraySize == rhs._arraySize) &&
               (_binding == rhs._binding);
    }
};

struct DescriptorSetLayoutDesc
{
    std::vector<DescriptorInfo> _descriptorInfos;
    VkShaderStageFlags _stageFlags = 0;

    inline bool operator==(const DescriptorSetLayoutDesc& rhs) const
    {
        return std::equal(_descriptorInfos.begin(), _descriptorInfos.end(),
                          rhs._descriptorInfos.begin(),
                          rhs._descriptorInfos.end()) &&
               (_stageFlags == rhs._stageFlags);
    }
};

}  // namespace VoxFlow

template <>
struct std::hash<VoxFlow::DescriptorInfo>
{
    std::size_t operator()(
        VoxFlow::DescriptorInfo const& info) const noexcept;
};

template <>
struct std::hash<VoxFlow::DescriptorSetLayoutDesc>
{
    std::size_t operator()(
        VoxFlow::DescriptorSetLayoutDesc const& setLayout) const noexcept;
};

#endif