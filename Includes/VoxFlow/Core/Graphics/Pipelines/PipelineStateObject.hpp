// Author : snowapril

#ifndef VOXEL_FLOW_PIPELINE_STATE_OBJECT_HPP
#define VOXEL_FLOW_PIPELINE_STATE_OBJECT_HPP

#include <volk/volk.h>
#include <VoxFlow/Core/Utils/NonCopyable.hpp>
#include <VoxFlow/Core/Graphics/RenderPass/RenderPassParams.hpp>
#include <VoxFlow/Core/Graphics/Pipelines/PipelineLayoutDescriptor.hpp>
#include <vector>

namespace VoxFlow
{

enum class VertexInputRate : uint8_t
{
    PerVertex = 0,
    PerInstance = 1,
    Undefined = 2,
    Count = Undefined,
};

struct PipelineInputLayout
{
    std::vector<VertexInputLayout> inputLayouts;

    PipelineInputLayout& addInputLayout(VertexInputLayout inputLayout)
    {
        inputLayouts.push_back(inputLayout);
        return *this;
    }
};

struct StencilOperationState
{
    VkStencilOp fail = VK_STENCIL_OP_KEEP;
    VkStencilOp pass = VK_STENCIL_OP_KEEP;
    VkStencilOp depthFail = VK_STENCIL_OP_KEEP;
    VkCompareOp compare = VK_COMPARE_OP_ALWAYS;
    uint32_t compareMask = 0xff;
    uint32_t writeMask = 0xff;
    uint32_t reference = 0xff;

};

struct DepthStencilCreation
{
    StencilOperationState front;
    StencilOperationState back;
    VkCompareOp depthComparison = VK_COMPARE_OP_ALWAYS;

    bool depthEnable = false;
    bool depthWriteEnable = false;
    bool stencilEnable = false;

    DepthStencilCreation& setDepth(bool write, VkCompareOp comparisonTest)
    {
        depthEnable = true;
        depthWriteEnable = write;
        depthComparison = comparisonTest;
        return *this;
    }
};

struct BlendState
{
    VkBlendFactor sourceColor = VK_BLEND_FACTOR_ONE;
    VkBlendFactor destinationColor = VK_BLEND_FACTOR_ONE;
    VkBlendOp colorOperation = VK_BLEND_OP_ADD;

    VkBlendFactor sourceAlpha = VK_BLEND_FACTOR_ONE;
    VkBlendFactor destinationAlpha = VK_BLEND_FACTOR_ONE;
    VkBlendOp alphaOperation = VK_BLEND_OP_ADD;

    VkColorComponentFlags colorMasks =
        VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
        VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

    bool blendEnabled = false;
    bool separateBlend = false;

    BlendState& setColor(VkBlendFactor sourceColor_,
        VkBlendFactor destinationColor_,
        VkBlendOp colorOperation_)
    {
        sourceColor = sourceColor_;
        destinationColor = destinationColor_;
        colorOperation = colorOperation_;
        blendEnabled = true;
        return *this;
    }

    BlendState& setAlpha(VkBlendFactor sourceAlpha_,
        VkBlendFactor destinationAlpha_,
        VkBlendOp alphaOperation_)
    {
        sourceAlpha = sourceAlpha_;
        destinationAlpha = destinationAlpha_;
        alphaOperation = alphaOperation_;
        separateBlend = true;
        return *this;
    }
    BlendState& setColorWriteMask(VkColorComponentFlags value)
    {
        colorMasks = value;
        return *this;
    }

};

struct BlendStateCreation
{
    BlendState blendStates[MAX_RENDER_TARGET_COUNTS];
    uint32_t activeStates = 0;

    BlendStateCreation& reset()
    { 
        activeStates = 0;
        return *this;
    }

    BlendState& addBlendState()
    {
        return blendStates[activeStates++];
    }
};

struct RasterizationCreation
{
    VkCullModeFlagBits cullMode = VK_CULL_MODE_NONE;
    VkFrontFace front = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    VkPolygonMode polygonMode = VK_POLYGON_MODE_FILL;
};

struct GraphicsPipelineState
{
    PipelineInputLayout inputLayout;
    RasterizationCreation rasterization;
    DepthStencilCreation depthStencil;
    BlendStateCreation blendState;

    VkPrimitiveTopology topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    VkPipelineCreateFlags flags = 0;
};
}  // namespace VoxFlow

#endif