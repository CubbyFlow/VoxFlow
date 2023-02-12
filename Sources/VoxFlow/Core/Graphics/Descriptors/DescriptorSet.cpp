// Author : snowapril

#include <VoxFlow/Core/Graphics/Descriptors/DescriptorSet.hpp>

namespace VoxFlow
{

DescriptorSet::DescriptorSet()
{
}

DescriptorSet::~DescriptorSet()
{
}

DescriptorSet::DescriptorSet(DescriptorSet&& other) noexcept
{
    operator=(std::move(other));
}

DescriptorSet& DescriptorSet::operator=(DescriptorSet&& other) noexcept
{
    if (&other != this)
    {
        // TODO(snowapril) :
    }
    return *this;
}

}  // namespace VoxFlow