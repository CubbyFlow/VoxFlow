// Author : snowapril

#include <VoxFlow/Core/FrameGraph/FrameGraphTexture.hpp>
#include <VoxFlow/Core/Resources/RenderResourceAllocator.hpp>

namespace VoxFlow
{

namespace RenderGraph
{
bool FrameGraphTexture::create(RenderResourceAllocator* resourceAllocator,
                               std::string_view&& debugName, Descriptor descriptor,
                               Usage usage)
{
    (void)resourceAllocator;
    (void)debugName;
    (void)descriptor;
    (void)usage;
    return true;
}

void FrameGraphTexture::destroy(RenderResourceAllocator* resourceAllocator)
{
    (void)resourceAllocator;
}
}  // namespace RenderGraph

}  // namespace VoxFlow