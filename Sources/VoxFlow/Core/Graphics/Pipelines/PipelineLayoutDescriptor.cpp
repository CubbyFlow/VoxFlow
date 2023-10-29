// Author : snowapril

#include <VoxFlow/Core/Graphics/Pipelines/PipelineLayoutDescriptor.hpp>
#include <VoxFlow/Core/Utils/Logger.hpp>

namespace VoxFlow
{

VkFormat VertexInputLayout::getVkFormat() const
{
    constexpr VkFormat FORMAT_TABLE[] = {
        VK_FORMAT_R16_SFLOAT, VK_FORMAT_R16G16_SFLOAT, VK_FORMAT_R16G16B16_SFLOAT, VK_FORMAT_R16G16B16A16_SFLOAT,
        VK_FORMAT_R32_SFLOAT, VK_FORMAT_R32G32_SFLOAT, VK_FORMAT_R32G32B32_SFLOAT, VK_FORMAT_R32G32B32A32_SFLOAT,
        VK_FORMAT_R64_SFLOAT, VK_FORMAT_R64G64_SFLOAT, VK_FORMAT_R64G64B64_SFLOAT, VK_FORMAT_R64G64B64A64_SFLOAT,
        VK_FORMAT_R16_SINT,   VK_FORMAT_R16G16_SINT,   VK_FORMAT_R16G16B16_SINT,   VK_FORMAT_R16G16B16A16_SINT,
        VK_FORMAT_R32_SINT,   VK_FORMAT_R32G32_SINT,   VK_FORMAT_R32G32B32_SINT,   VK_FORMAT_R32G32B32A32_SINT,
        VK_FORMAT_R64_SINT,   VK_FORMAT_R64G64_SINT,   VK_FORMAT_R64G64B64_SINT,   VK_FORMAT_R64G64B64A64_SINT,
        VK_FORMAT_R16_UINT,   VK_FORMAT_R16G16_UINT,   VK_FORMAT_R16G16B16_UINT,   VK_FORMAT_R16G16B16A16_UINT,
        VK_FORMAT_R32_UINT,   VK_FORMAT_R32G32_UINT,   VK_FORMAT_R32G32B32_UINT,   VK_FORMAT_R32G32B32A32_UINT,
        VK_FORMAT_R64_UINT,   VK_FORMAT_R64G64_UINT,   VK_FORMAT_R64G64B64_UINT,   VK_FORMAT_R64G64B64A64_UINT,
    };
    static_assert((sizeof(FORMAT_TABLE) / sizeof(VkFormat)) == 36);

    VkFormat resultFormat = VK_FORMAT_UNDEFINED;
    switch (_baseType)
    {
        case VertexFormatBaseType::Float16:
            resultFormat = FORMAT_TABLE[((_stride / 2) - 1)];
            break;
        case VertexFormatBaseType::Float32:
            resultFormat = FORMAT_TABLE[((_stride / 4) - 1) + 4];
            break;
        case VertexFormatBaseType::Float48:
            resultFormat = FORMAT_TABLE[((_stride / 6) - 1) + 4];
            break;
        case VertexFormatBaseType::Float64:
            resultFormat = FORMAT_TABLE[((_stride / 8) - 1) + 8];
            break;
        case VertexFormatBaseType::Int16:
            resultFormat = FORMAT_TABLE[((_stride / 2) - 1) + 12];
            break;
        case VertexFormatBaseType::Int32:
            resultFormat = FORMAT_TABLE[((_stride / 4) - 1) + 16];
            break;
        case VertexFormatBaseType::Int48:
            resultFormat = FORMAT_TABLE[((_stride / 6) - 1) + 20];
            break;
        case VertexFormatBaseType::Int64:
            resultFormat = FORMAT_TABLE[((_stride / 8) - 1) + 20];
            break;
        case VertexFormatBaseType::Uint16:
            resultFormat = FORMAT_TABLE[((_stride / 2) - 1) + 24];
            break;
        case VertexFormatBaseType::Uint32:
            resultFormat = FORMAT_TABLE[((_stride / 4) - 1) + 28];
            break;
        case VertexFormatBaseType::Uint48:
            resultFormat = FORMAT_TABLE[((_stride / 6) - 1) + 32];
            break;
        case VertexFormatBaseType::Uint64:
            resultFormat = FORMAT_TABLE[((_stride / 8) - 1) + 32];
            break;
        case VertexFormatBaseType::Unknown:
            VOX_ASSERT(false, "Unknown vertex format must be handled");
            break;
    }
    return resultFormat;
}

ShaderReflectionDataGroup::ShaderReflectionDataGroup(const ShaderReflectionDataGroup& rhs)
{
    operator=(rhs);
}

ShaderReflectionDataGroup::ShaderReflectionDataGroup(ShaderReflectionDataGroup&& rhs)
{
    operator=(std::move(rhs));
}

ShaderReflectionDataGroup& ShaderReflectionDataGroup::operator=(const ShaderReflectionDataGroup& rhs)
{
    if (this != &rhs)
    {
        _descriptors = rhs._descriptors;
        _vertexInputLayouts = rhs._vertexInputLayouts;
        _fragmentOutputLayouts = rhs._fragmentOutputLayouts;
        _stageFlagBit = rhs._stageFlagBit;
    }
    return *this;
}

ShaderReflectionDataGroup& ShaderReflectionDataGroup::operator=(ShaderReflectionDataGroup&& rhs)
{
    if (this != &rhs)
    {
        _descriptors.swap(rhs._descriptors);
        _vertexInputLayouts.swap(rhs._vertexInputLayouts);
        _fragmentOutputLayouts.swap(rhs._fragmentOutputLayouts);
        _stageFlagBit = rhs._stageFlagBit;
    }
    return *this;
}

}  // namespace VoxFlow

std::size_t std::hash<VoxFlow::VertexInputLayout>::operator()(VoxFlow::VertexInputLayout const& inputLayout) const noexcept
{
    uint32_t seed = 0;

    VoxFlow::hash_combine(seed, inputLayout._location);
    VoxFlow::hash_combine(seed, inputLayout._stride);
    VoxFlow::hash_combine(seed, static_cast<uint32_t>(inputLayout._baseType));

    return seed;
}

std::size_t std::hash<VoxFlow::FragmentOutputLayout>::operator()(VoxFlow::FragmentOutputLayout const& outputLayout) const noexcept
{
    uint32_t seed = 0;

    VoxFlow::hash_combine(seed, outputLayout._location);
    VoxFlow::hash_combine(seed, static_cast<uint32_t>(outputLayout._format));

    return seed;
}

std::size_t std::hash<VoxFlow::PipelineLayoutDescriptor>::operator()(VoxFlow::PipelineLayoutDescriptor const& shaderLayout) const noexcept
{
    uint32_t seed = 0;

    for (const VoxFlow::DescriptorSetLayoutDesc& desc : shaderLayout._sets)
    {
        VoxFlow::hash_combine(seed, desc);
    }

    VoxFlow::hash_combine(seed, shaderLayout._pushConstantSize);
    return seed;
}