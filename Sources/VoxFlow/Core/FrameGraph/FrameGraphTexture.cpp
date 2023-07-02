// Author : snowapril

#include <VoxFlow/Core/FrameGraph/FrameGraphTexture.hpp>
#include <VoxFlow/Core/Resources/RenderResourceAllocator.hpp>

namespace VoxFlow
{

namespace FrameGraph
{
bool FrameGraphTexture::create(RenderResourceAllocator* resourceAllocator,
                               std::string&& debugName, Descriptor descriptor,
                               Usage usage)
{
    (void)resourceAllocator;
    (void)debugName;
    (void)descriptor;
    (void)usage;
    return true;
}

void FrameGraphTexture::release(RenderResourceAllocator* resourceAllocator)
{
    (void)resourceAllocator;
}
}  // namespace FrameGraph

}  // namespace VoxFlow