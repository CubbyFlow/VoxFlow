// Author : snowapril

#include <VoxFlow/Core/Graphics/Descriptors/DescriptorPool.hpp>
#include <utility>

namespace VoxFlow
{

DescriptorPool::DescriptorPool()
{
}

DescriptorPool::~DescriptorPool()
{
}

DescriptorPool::DescriptorPool(DescriptorPool&& other) noexcept
{
    operator=(std::move(other));
}

DescriptorPool& DescriptorPool::operator=(DescriptorPool&& other) noexcept
{
    if (&other != this)
    {
        // TODO(snowapril) :
    }
    return *this;
}

}  // namespace VoxFlow