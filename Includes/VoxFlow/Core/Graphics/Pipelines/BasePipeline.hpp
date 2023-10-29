// Author : snowapril

#ifndef VOXEL_FLOW_BASE_PIPELINE_HPP
#define VOXEL_FLOW_BASE_PIPELINE_HPP

#include <volk/volk.h>
#include <VoxFlow/Core/Graphics/Commands/CommandBuffer.hpp>
#include <VoxFlow/Core/Graphics/Pipelines/ShaderUtil.hpp>
#include <VoxFlow/Core/Utils/NonCopyable.hpp>
#include <memory>
#include <vector>

namespace VoxFlow
{
class ShaderModule;
class LogicalDevice;
class PipelineLayout;
class PipelineStreamingContext;
class PipelineCache;

class BasePipeline : NonCopyable
{
 public:
    explicit BasePipeline(PipelineStreamingContext* pipelineStreamingContext, std::vector<ShaderPathInfo>&& shaderFilePaths);
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

    /**
     * @brief set pipeline cache for this pipeline
     */
    void setPipelineCache(std::unique_ptr<PipelineCache>&& pipelineCache);

 protected:
    /**
     * release shader modules and pipeline layout which is used to create
     * pipeline and also release vulkan pipeline object
     */
    void release();

    /**
     * export pipeline cache after creation via pipeline streaming context
     */
    void exportPipelineCache();

 protected:
    PipelineStreamingContext* _pipelineStreamingContext = nullptr;
    LogicalDevice* _logicalDevice = nullptr;
    std::unique_ptr<PipelineLayout> _pipelineLayout;
    std::unique_ptr<PipelineCache> _pipelineCache;
    std::vector<std::unique_ptr<ShaderModule>> _shaderModules;
    VkPipeline _pipeline{ VK_NULL_HANDLE };
};
}  // namespace VoxFlow

#endif