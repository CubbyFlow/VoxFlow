// Author : snowapril

#ifndef VOXEL_FLOW_COMMAND_JOB_SYSTEM_IMPL_HPP
#define VOXEL_FLOW_COMMAND_JOB_SYSTEM_IMPL_HPP

#include <VoxFlow/Core/Graphics/Commands/CommandJobSystem.hpp>
#include <VoxFlow/Core/Graphics/Commands/CommandBuffer.hpp>
#include <VoxFlow/Core/Graphics/Commands/CommandBuffer.hpp>
#include <VoxFlow/Core/Graphics/Pipelines/ResourceBindingLayout.hpp>
#include <type_traits>

namespace VoxFlow
{

template <typename...>
struct CommandParameterIndex;

template <typename T, typename... R>
struct CommandParameterIndex<T, T, R...> : std::integral_constant<size_t, 0>
{
};

template <typename T, typename F, typename... R>
struct CommandParameterIndex<T, F, R...>
    : std::integral_constant<size_t, 1 + CommandParameterIndex<T, R...>::value>
{
};

class CommandParameterPacker
{
 public:
    template <typename... ParamTypes>
    CommandParameterPacker(ParamTypes&&... params)
        : _params{ static_cast<void*>(&params)... }
    {
    }

     template <typename ParamType>
     ParamType getParam( const uint32_t index )
    {
         return *(static_cast<ParamType*>(_params[index]));
    }

private:
    std::vector<void*> _params;
};

template <typename... CommandJobArgs>
void CommandStream::addJob(CommandJobType jobType, CommandJobArgs&&... args)
{
    CommandBuffer* cmdBuffer = getOrAllocateCommandBuffer();

    CommandParameterPacker params(std::forward<CommandJobArgs>(args)...);

    switch (jobType)
    {
        case CommandJobType::BeginRenderPass:
            cmdBuffer->beginRenderPass(params.getParam<AttachmentGroup>(0),
                                       params.getParam<RenderPassParams>(1));
            break;
        case CommandJobType::EndRenderPass:
            cmdBuffer->endRenderPass();
            break;
        case CommandJobType::BindPipeline:
            cmdBuffer->bindPipeline(params.getParam<BasePipeline*>(0));
            break;

        case CommandJobType::SetViewport:
            cmdBuffer->setViewport(params.getParam<glm::uvec2>(0));
            break;

        case CommandJobType::BindResourceGroup:
            cmdBuffer->bindResourceGroup(
                params.getParam<SetSlotCategory>(0),
                params.getParam<std::vector<ShaderVariableBinding>>(1));
            break;

        case CommandJobType::UploadBuffer:
            cmdBuffer->uploadBuffer(params.getParam<Buffer*>(0),
                                    params.getParam<StagingBuffer*>(1),
                                    params.getParam<uint32_t>(2),
                                    params.getParam<uint32_t>(3),
                                    params.getParam<uint32_t>(4));
            break;

        case CommandJobType::UploadTexture:
            cmdBuffer->uploadTexture(params.getParam<Texture*>(0),
                                     params.getParam<StagingBuffer*>(1),
                                     params.getParam<uint32_t>(2),
                                     params.getParam<uint32_t>(3),
                                     params.getParam<uint32_t>(4));
            break;

        case CommandJobType::Draw:
            cmdBuffer->draw(params.getParam<uint32_t>(0), 
                            params.getParam<uint32_t>(1),
                            params.getParam<uint32_t>(2), 
                            params.getParam<uint32_t>(3));
            break;

        case CommandJobType::DrawIndexed:
            cmdBuffer->drawIndexed(
                params.getParam<uint32_t>(0), params.getParam<uint32_t>(1),
                params.getParam<uint32_t>(2), params.getParam<uint32_t>(3),
                params.getParam<uint32_t>(4));
            break;

        case CommandJobType::MakeSwapChainFinalLayout:
            cmdBuffer->makeSwapChainFinalLayout(params.getParam<SwapChain*>(0),
                                                params.getParam<uint32_t>(1));
            break;

        case CommandJobType::BindVertexBuffer:
            cmdBuffer->bindVertexBuffer(params.getParam<Buffer*>(0));
            break;

        case CommandJobType::BindIndexBuffer:
            cmdBuffer->bindIndexBuffer(params.getParam<Buffer*>(0));
            break;
    }
}
}  // namespace VoxFlow

#endif