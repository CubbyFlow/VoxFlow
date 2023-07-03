// Author : snowapril

#ifndef VOXEL_FLOW_DESCRIPTOR_SET_HPP
#define VOXEL_FLOW_DESCRIPTOR_SET_HPP

#include <VoxFlow/Core/Utils/RendererCommon.hpp>
#include <memory>
#include <unordered_map>
#include <vector>
#include <variant>

namespace VoxFlow
{

struct DescriptorSetLayoutDesc
{
    struct CombinedImage
    {
        VkFormat _format = VK_FORMAT_UNDEFINED;
        uint32_t _arraySize = 0;
        uint32_t _binding = 0;

        inline bool operator==(const CombinedImage& rhs) const
        {
            return (_format == rhs._format) && (_arraySize == rhs._arraySize) &&
                   (_binding == rhs._binding);   
        }
    };
    struct UniformBuffer
    {
        uint32_t _size = 0;
        uint32_t _arraySize = 0;
        uint32_t _binding = 0;

        inline bool operator==(const UniformBuffer& rhs) const
        {
            return (_size == rhs._size) && (_arraySize == rhs._arraySize) &&
                   (_binding == rhs._binding);
        }
    };
    struct StorageBuffer
    {
        uint32_t _size = 0;
        uint32_t _arraySize = 0;
        uint32_t _binding = 0;

        inline bool operator==(const StorageBuffer& rhs) const
        {
            return (_size == rhs._size) && (_arraySize == rhs._arraySize) &&
                   (_binding == rhs._binding);
        }
    };

    using DescriptorType =
        std::variant<CombinedImage, UniformBuffer, StorageBuffer>;
    using ContainerType = std::unordered_map<std::string, DescriptorType>;

    ContainerType _bindingMap;
    VkShaderStageFlags _stageFlags = 0;

    inline bool operator==(const DescriptorSetLayoutDesc& rhs) const
    {
        return std::equal(_bindingMap.begin(), _bindingMap.end(),
                          rhs._bindingMap.begin(), rhs._bindingMap.end()) &&
               (_stageFlags == rhs._stageFlags);
    }
};

}  // namespace VoxFlow

template <>
struct std::hash<VoxFlow::DescriptorSetLayoutDesc>
{
    std::size_t operator()(
        VoxFlow::DescriptorSetLayoutDesc const& setLayout) const noexcept;
};

#endif