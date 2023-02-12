// Author : snowapril

#ifndef VOXEL_FLOW_SHADER_RESOURCE_COLLECTION_HPP
#define VOXEL_FLOW_SHADER_RESOURCE_COLLECTION_HPP

#include <VoxFlow/Core/Utils/NonCopyable.hpp>
#include <memory>
#include <unordered_map>
#include <vector>

namespace VoxFlow
{
    class LogicalDevice;
class DescriptorSetLayout;
class ShaderModule;

class ShaderResourceCollector : private NonCopyable
{
 public:
    explicit ShaderResourceCollector(LogicalDevice* logicalDevice);
    ~ShaderResourceCollector();
    ShaderResourceCollector(ShaderResourceCollector&& rhs);
    ShaderResourceCollector& operator=(ShaderResourceCollector&& rhs);

 public:
     // Get or create new descriptor set layouts for given shader modules
    std::vector<std::shared_ptr<DescriptorSetLayout>>
    getOrCreateRequiredSetLayout(
        const std::vector<std::shared_ptr<ShaderModule>>& shaderModules);

 private:
    LogicalDevice* _logicalDevice = nullptr;
    std::unordered_map<uint64_t, std::shared_ptr<DescriptorSetLayout>>
        _descriptorSetLayoutCollection;
};

}  // namespace VoxFlow

#endif