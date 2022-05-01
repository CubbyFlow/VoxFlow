// Author : snowapril

#ifndef VOXEL_FLOW_BASE_PIPELINE_HPP
#define VOXEL_FLOW_BASE_PIPELINE_HPP

#include <VoxFlow/Core/Utils/NonCopyable.hpp>
#include <VoxFlow/Core/Utils/pch.hpp>
#include <memory>

namespace VoxFlow
{
class LogicalDevice;

class BasePipeline : NonCopyable
{
 public:
    explicit BasePipeline(const std::shared_ptr<LogicalDevice>& device);
    ~BasePipeline() override;
    BasePipeline(BasePipeline&& other) noexcept;
    BasePipeline& operator=(BasePipeline&& other) noexcept;

    [[nodiscard]] VkPipeline get() const noexcept
    {
        return _pipeline;
    }

 protected:
    void release();

 protected:
    std::shared_ptr<LogicalDevice> _device;
    VkPipeline _pipeline{ VK_NULL_HANDLE };
};
}  // namespace VoxFlow

#endif