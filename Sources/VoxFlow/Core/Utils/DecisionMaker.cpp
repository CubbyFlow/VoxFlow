// Author : snowapril

#include <spdlog/spdlog.h>
#include <VoxFlow/Core/Utils/DecisionMaker.hpp>

namespace VoxFlow
{
VkResult DecisionMaker::pickLayers(std::vector<const char*>& used, const std::vector<VkLayerProperties>& properties,
                                   const std::vector<Context::Entry>& requested)
{
    for (const auto& entry : requested)
    {
        if (auto ranges =
                std::find_if(properties.begin(), properties.end(), [&entry](const VkLayerProperties& p) { return strcmp(entry.entryName, p.layerName) == 0; });
            ranges != properties.end())
        {
            used.emplace_back(entry.entryName);
        }
        else if (entry.isOptional == false)
        {
            spdlog::error("Requested Vulkan Layer {} not found.", entry.entryName);
            return VK_ERROR_LAYER_NOT_PRESENT;
        }
    }
    return VK_SUCCESS;
}

VkResult DecisionMaker::pickExtensions(std::vector<const char*>& used, const std::vector<VkExtensionProperties>& properties,
                                       const std::vector<Context::Entry>& requested, std::vector<void*>& featureStructs)
{
    for (const auto& entry : requested)
    {
        if (auto ranges = std::find_if(properties.begin(), properties.end(),
                                       [&entry](const VkExtensionProperties& p) { return strcmp(entry.entryName, p.extensionName) == 0; });
            ranges != properties.end())
        {
            used.emplace_back(entry.entryName);
            if (entry.pFeatureStruct != nullptr)
            {
                featureStructs.push_back(entry.pFeatureStruct);
            }
        }
        else if (entry.isOptional == false)
        {
            spdlog::error("Requested Vulkan Extension '{}' not found.", entry.entryName);
            return VK_ERROR_EXTENSION_NOT_PRESENT;
        }
    }
    return VK_SUCCESS;
}
}  // namespace VoxFlow