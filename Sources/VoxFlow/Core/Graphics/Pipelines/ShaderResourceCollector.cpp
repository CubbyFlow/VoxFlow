// Author : snowapril

#include <VoxFlow/Core/Graphics/Descriptors/DescriptorSetLayout.hpp>
#include <VoxFlow/Core/Graphics/Pipelines/ShaderLayoutBinding.hpp>
#include <VoxFlow/Core/Graphics/Pipelines/ShaderModule.hpp>
#include <VoxFlow/Core/Graphics/Pipelines/ShaderResourceCollector.hpp>
#include <algorithm>
#include <glm/common.hpp>

namespace VoxFlow
{
ShaderResourceCollector::ShaderResourceCollector(LogicalDevice* logicalDevice)
    : _logicalDevice(logicalDevice)
{
}

ShaderResourceCollector::~ShaderResourceCollector()
{
}

ShaderResourceCollector::ShaderResourceCollector(ShaderResourceCollector&& rhs)
{
    operator=(std::move(rhs));
}

ShaderResourceCollector& ShaderResourceCollector::operator=(
    ShaderResourceCollector&& rhs)
{
    if (this != &rhs)
    {
        _descriptorSetLayoutCollection.swap(rhs._descriptorSetLayoutCollection);
    }
    return *this;
}

std::vector<std::shared_ptr<DescriptorSetLayout>>
ShaderResourceCollector::getOrCreateRequiredSetLayout(
    const std::vector<std::shared_ptr<ShaderModule>>& shaderModules)
{
    std::vector<ShaderLayoutBinding> unorderedLayoutBindings;
    std::for_each(shaderModules.begin(), shaderModules.end(),
                  [&unorderedLayoutBindings](
                      const std::shared_ptr<ShaderModule>& shaderModule) {
                      const std::vector<ShaderLayoutBinding> layoutBindings =
                          shaderModule->getShaderLayoutBindings();
                      unorderedLayoutBindings.insert(
                          unorderedLayoutBindings.end(), layoutBindings.begin(),
                          layoutBindings.end());
                  });

    uint32_t maxSetNumber = 0;
    std::vector<std::pair<uint64_t, std::vector<ShaderLayoutBinding>>>
        collectedLayoutBinding;

    std::for_each(unorderedLayoutBindings.begin(),
                  unorderedLayoutBindings.end(),
                  [&maxSetNumber](const ShaderLayoutBinding& layoutBinding) {
                      maxSetNumber = glm::max(maxSetNumber, layoutBinding._set);
                  });

    collectedLayoutBinding.resize(
        maxSetNumber, std::make_pair(0, std::vector<ShaderLayoutBinding>()));
    std::for_each(
        unorderedLayoutBindings.begin(), unorderedLayoutBindings.end(),
        [&collectedLayoutBinding](const ShaderLayoutBinding& layoutBinding) {
            collectedLayoutBinding[layoutBinding._set].first ^=
                layoutBinding._hashCached;
            collectedLayoutBinding[layoutBinding._set].second.push_back(
                layoutBinding);
        });

    std::vector<std::shared_ptr<DescriptorSetLayout>> setLayoutCollection;
    for (size_t i = 0; i < collectedLayoutBinding.size(); ++i)
    {
        const std::pair<uint64_t, std::vector<ShaderLayoutBinding>>&
            layoutBindingsPerSet = collectedLayoutBinding[i];

        auto it =
            _descriptorSetLayoutCollection.find(layoutBindingsPerSet.first);
        if (it == _descriptorSetLayoutCollection.end())
        {
            std::shared_ptr<DescriptorSetLayout> newSetLayout =
                std::make_shared<DescriptorSetLayout>(
                    _logicalDevice, layoutBindingsPerSet.second);
            _descriptorSetLayoutCollection.insert(
                std::make_pair(layoutBindingsPerSet.first, newSetLayout));
            setLayoutCollection.push_back(newSetLayout);
        }
        else
        {
            setLayoutCollection.push_back(it->second);
        }
    }

    return setLayoutCollection;
}

}  // namespace VoxFlow