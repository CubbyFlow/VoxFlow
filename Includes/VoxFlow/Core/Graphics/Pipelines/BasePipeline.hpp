// Author : snowapril

#ifndef VOXEL_FLOW_BASE_PIPELINE_HPP
#define VOXEL_FLOW_BASE_PIPELINE_HPP

#include <volk/volk.h>
#include <VoxFlow/Core/Graphics/Commands/CommandBuffer.hpp>
#include <VoxFlow/Core/Utils/NonCopyable.hpp>
#include <memory>
#include <initializer_list>
#include <vector>

namespace VoxFlow
{
class ShaderModule;
class LogicalDevice;
class PipelineLayout;

class BasePipeline : NonCopyable
{
 public:
    explicit BasePipeline(LogicalDevice* logicalDevice,
                          std::initializer_list<const char*>&& shaderFilePaths);
    ~BasePipeline() override;
    BasePipeline(BasePipeline&& other) noexcept;
    BasePipeline& operator=(BasePipeline&& other) noexcept;

    /**
     * @return vulkan pipeline handle created with given shader paths
     */
    [[nodiscard]] inline VkPipeline get() const noexcept
    {
        return _pipeline;
    }

    /**
     * @return whether pipeline was created or not
     */
    [[nodiscard]] inline bool validatePipeline() const noexcept
    {
        return _pipeline != VK_NULL_HANDLE;
    }

    /**
     * @return pipeline layout which is created by descriptor set layouts
     * containing all shader resource binding layout reflected
     */
    [[nodiscard]] inline PipelineLayout* getPipelineLayout() const noexcept
    {
        return _pipelineLayout.get();
    }

    /**
     * @return binding point of this pipeline. It will be compute or graphics
     * pipeline bind point.
     */
    [[nodiscard]] virtual VkPipelineBindPoint getBindPoint() const noexcept = 0;

 protected:
    /**
     * Initialize pipeline layout with combined shader layout bindings.
     * @return whether pipeline layout initialization success or not
     */
    bool initializePipelineLayout();

    /**
     * release shader modules and pipeline layout which is used to create
     * pipeline and also release vulkan pipeline object
     */
    void release();

 protected:
    LogicalDevice* _logicalDevice = nullptr;
    std::unique_ptr<PipelineLayout> _pipelineLayout;
    std::vector<std::unique_ptr<ShaderModule>> _shaderModules;
    VkPipeline _pipeline{ VK_NULL_HANDLE };
};
}  // namespace VoxFlow

#endif