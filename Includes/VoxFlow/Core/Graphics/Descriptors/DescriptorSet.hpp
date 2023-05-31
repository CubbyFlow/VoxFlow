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
    struct SampledImage
    {
        VkFormat _format = VK_FORMAT_UNDEFINED;
        uint32_t _arraySize = 0;
        uint32_t _binding = 0;

        inline bool operator==(const SampledImage& rhs) const = default;
    };
    struct UniformBuffer
    {
        uint32_t _size = 0;
        uint32_t _arraySize = 0;
        uint32_t _binding = 0;

        inline bool operator==(const UniformBuffer& rhs) const = default;
    };
    struct StorageBuffer
    {
        uint32_t _size = 0;
        uint32_t _arraySize = 0;
        uint32_t _binding = 0;

        inline bool operator==(const StorageBuffer& rhs) const = default;
    };

    using DescriptorType =
        std::variant<SampledImage, UniformBuffer, StorageBuffer>;
    using ContainerType = std::unordered_map<std::string, DescriptorType>;

    ContainerType _bindingMap;
    VkShaderStageFlags _stageFlags = 0;

    inline bool operator==(const DescriptorSetLayoutDesc& rhs) const
    {
        return _bindingMap == rhs._bindingMap && _stageFlags == rhs._stageFlags;
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