// Author : snowapril

#ifndef VOXEL_FLOW_COMMAND_JOB_SYSTEM_HPP
#define VOXEL_FLOW_COMMAND_JOB_SYSTEM_HPP

#include <volk/volk.h>
#include <VoxFlow/Core/Graphics/Commands/CommandConfig.hpp>
#include <VoxFlow/Core/Utils/FenceObject.hpp>
#include <VoxFlow/Core/Utils/NonCopyable.hpp>
#include <VoxFlow/Core/Utils/RendererCommon.hpp>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <unordered_map>

namespace VoxFlow
{
class CommandPool;
class LogicalDevice;
class CommandBuffer;
class SwapChain;
class CommandPool;
class Queue;

enum class CommandJobType
{
    BeginRenderPass,
    EndRenderPass,
    BindPipeline,
    SetViewport,
    BindResourceGroup,
    UploadBuffer,
    UploadTexture,
    Draw,
    DrawIndexed,
    MakeSwapChainFinalLayout,
    BindVertexBuffer,
    BindIndexBuffer
};

class CommandStream final : private NonCopyable
{
 public:
    using CommandPoolStorage =
        std::unordered_map<std::thread::id, std::unique_ptr<CommandPool>>;
    using CommandBufferStorage =
        std::unordered_map<std::thread::id, std::shared_ptr<CommandBuffer>>;

 public:
    explicit CommandStream(LogicalDevice* logicalDevice, Queue* queue);
    ~CommandStream();

    FenceObject flush(SwapChain* swapChain, const FrameContext* frameContext,
                      const bool waitAllCompletion);

    template <typename... CommandJobArgs>
    void addJob(CommandJobType jobType, CommandJobArgs&&... args);

private:
    CommandBuffer* getOrAllocateCommandBuffer();
    CommandPool* getOrAllocateCommandPool();

 private:
    std::recursive_mutex _streamMutex;
    CommandPoolStorage _cmdPoolStorage;
    CommandBufferStorage _cmdBufferStorage;
    LogicalDevice* _logicalDevice = nullptr;
    Queue* _queue = nullptr;
};

class CommandJobSystem final : private NonCopyable
{
 public:
    using CommandStreamPtr = std::unique_ptr<CommandStream>;
    using CommandStreamMap = std::unordered_map<CommandStreamKey, CommandStreamPtr>;

 public:
    explicit CommandJobSystem(LogicalDevice* logicalDevice);
    ~CommandJobSystem() override;

    void createCommandStream(const CommandStreamKey& streamKey, Queue* queue);
    CommandStream* getCommandStream(const CommandStreamKey& streamKey);

 private:
    void processJob();

 protected:
    LogicalDevice* _logicalDevice = nullptr;
    CommandStreamMap _cmdStreams;
};

}  // namespace VoxFlow

#include <VoxFlow/Core/Graphics/Commands/CommandJobSystem-Impl.hpp>

#endif