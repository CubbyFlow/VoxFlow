// Author : snowapril

#include <VoxFlow/Core/Devices/LogicalDevice.hpp>
#include <VoxFlow/Core/Resources/Sampler.hpp>

namespace VoxFlow
{
Sampler::Sampler(std::string_view&& debugName, LogicalDevice* logicalDevice) : RenderResource(std::move(debugName), logicalDevice, nullptr)
{
}

Sampler::~Sampler()
{
    release();
}

bool Sampler::initialize()
{
    // TODO(snowapril) : implement various type of samplers
    VkSamplerCreateInfo samplerInfo{};
    samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerInfo.magFilter = VK_FILTER_LINEAR;
    samplerInfo.minFilter = VK_FILTER_LINEAR;
    samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.anisotropyEnable = VK_FALSE;

    VK_ASSERT(vkCreateSampler(_logicalDevice->get(), &samplerInfo, nullptr, &_vkSampler));

#if defined(VK_DEBUG_NAME_ENABLED)
    DebugUtil::setObjectName(_logicalDevice, _vkSampler, _debugName.c_str());
#endif

    return true;
}

void Sampler::release()
{
    if (_vkSampler != VK_NULL_HANDLE)
    {
        vkDestroySampler(_logicalDevice->get(), _vkSampler, nullptr);
    }
}
}  // namespace VoxFlow
