// Author : snowapril

#ifndef VOXEL_FLOW_DECISION_MAKER_HPP
#define VOXEL_FLOW_DECISION_MAKER_HPP

#include <volk/volk.h>
#include <VoxFlow/Core/Devices/Context.hpp>
#include <string>
#include <vector>

namespace VoxFlow
{
class DecisionMaker
{
 public:
    DecisionMaker() = delete;

    [[nodiscard]] static VkResult pickLayers(
        std::vector<const char*>& used,
        const std::vector<VkLayerProperties>& properties,
        const std::vector<Context::Entry>& requested);

    [[nodiscard]] static VkResult pickExtensions(
        std::vector<const char*>& used,
        const std::vector<VkExtensionProperties>& properties,
        const std::vector<Context::Entry>& requested,
        std::vector<void*>& featureStructs);
};
}  // namespace VoxFlow

#endif