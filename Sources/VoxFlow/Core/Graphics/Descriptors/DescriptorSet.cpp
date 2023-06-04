// Author : snowapril

#include <VoxFlow/Core/Graphics/Descriptors/DescriptorSet.hpp>
#include <VoxFlow/Core/Utils/RendererCommon.hpp>

namespace VoxFlow
{

}  // namespace VoxFlow

std::size_t std::hash<VoxFlow::DescriptorSetLayoutDesc>::operator()(
    VoxFlow::DescriptorSetLayoutDesc const& setLayout) const noexcept
{
    uint32_t seed = 0;

    VoxFlow::DescriptorSetLayoutDesc::ContainerType::const_iterator it =
        setLayout._bindingMap.begin();

    for (; it != setLayout._bindingMap.end(); ++it)
    {
        std::visit(
            VoxFlow::overloaded{
                [&seed](VoxFlow::DescriptorSetLayoutDesc::SampledImage setBinding) {
                    VoxFlow::hash_combine(seed, setBinding._arraySize);
                    VoxFlow::hash_combine(
                        seed, static_cast<uint32_t>(setBinding._format));
                    VoxFlow::hash_combine(seed, setBinding._binding);
                },
                [&seed](VoxFlow::DescriptorSetLayoutDesc::UniformBuffer setBinding) {
                    VoxFlow::hash_combine(seed, setBinding._arraySize);
                    VoxFlow::hash_combine(seed, setBinding._binding);
                    VoxFlow::hash_combine(seed, setBinding._size);
                },
                [&seed](VoxFlow::DescriptorSetLayoutDesc::StorageBuffer setBinding) {
                    VoxFlow::hash_combine(seed, setBinding._arraySize);
                    VoxFlow::hash_combine(seed, setBinding._binding);
                    VoxFlow::hash_combine(seed, setBinding._size);
                },
            },
            it->second);
    }

    VoxFlow::hash_combine(seed, static_cast<uint32_t>(setLayout._stageFlags));
    return seed;
}