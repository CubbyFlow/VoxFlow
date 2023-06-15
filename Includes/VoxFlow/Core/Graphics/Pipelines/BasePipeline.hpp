// Author : snowapril

#ifndef VOXEL_FLOW_BASE_PIPELINE_HPP
#define VOXEL_FLOW_BASE_PIPELINE_HPP

#include <volk/volk.h>
#include <VoxFlow/Core/Graphics/Commands/CommandBuffer.hpp>
#include <VoxFlow/Core/Utils/NonCopyable.hpp>
#include <memory>

namespace VoxFlow
{
class ShaderModule;
class LogicalDevice;
class PipelineLayout;

class BasePipeline : NonCopyable
{
 public:
    explicit BasePipeline(
        LogicalDevice* logicalDevice,
        std::vector<const char*>&& shaderFilePaths);
    ~BasePipeline() override;
    BasePipeline(BasePipeline&& other) noexcept;
    BasePipeline& operator=(BasePipeline&& other) noexcept;

    [[nodiscard]] VkPipeline get() const noexcept
    {
        return _pipeline;
    }

    [[nodiscard]] inline PipelineLayout* getPipelineLayout() const noexcept
    {
        return _pipelineLayout.get();
    }

    [[nodiscard]] virtual VkPipelineBindPoint getBindPoint() const noexcept = 0;

 protected:

    /**
     * Initialize pipeline layout with combined shader layout bindings.
     * @return whether pipeline layout initialization success or not
     */
    bool initializePipelineLayout();

    void release();

 protected:
    LogicalDevice* _logicalDevice = nullptr;
    std::unique_ptr<PipelineLayout> _pipelineLayout;
    std::vector<std::unique_ptr<ShaderModule>> _shaderModules;
    VkPipeline _pipeline{ VK_NULL_HANDLE };
};
}  // namespace VoxFlow

#endif