// Author : snowapril

#include <VoxFlow/Core/FrameGraph/FrameGraph.hpp>
#include <VoxFlow/Core/FrameGraph/FrameGraphTexture.hpp>
#include <VoxFlow/Core/Graphics/Commands/CommandJobSystem.hpp>
#include <sstream>
#include <string>
#include <fstream>
#include "../../UnitTestUtils.hpp"

TEST_CASE("FrameGraph")
{
    using namespace VoxFlow;

    RenderGraph::FrameGraph frameGraph;
    RenderGraph::BlackBoard& blackBoard = frameGraph.getBlackBoard();

    {
        RenderGraph::ResourceHandle backBuffer = frameGraph.importRenderTarget(
            "BackBuffer",
            RenderGraph::FrameGraphTexture::Descriptor{
                ._width = 1200,
                ._height = 900,
                ._depth = 1,
                ._level = 1,
                ._sampleCounts = 1,
                ._format = VK_FORMAT_R8G8B8A8_UNORM },

            RenderGraph::FrameGraphRenderPass::ImportedDescriptor{
                ._attachmentSlot = AttachmentMaskFlags::All,
                ._viewportSize = glm::uvec2(1200, 900),
                ._clearColor = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f),
                ._clearFlags = AttachmentMaskFlags::All,
                ._writableAttachment = AttachmentMaskFlags::All,
                ._numSamples = 1,
            },
            nullptr);

        blackBoard["BackBuffer"] = backBuffer;
    }

    struct SamplePassData1
    {
        RenderGraph::ResourceHandle _input1;
        RenderGraph::ResourceHandle _input2;
        RenderGraph::ResourceHandle _output1;
        bool _isExecuted = false;
    };

    const SamplePassData1& samplePass1 =
        frameGraph.addCallbackPass<SamplePassData1>(
            "Sample Pass 1",
            [&](RenderGraph::FrameGraphBuilder& builder,
                SamplePassData1& passData) {
                passData._input1 =
                    builder.allocate<RenderGraph::FrameGraphTexture>(
                        "Sample Pass 1 Resource Input 1",
                        RenderGraph::FrameGraphTexture::Descriptor{
                            ._width = 1200,
                            ._height = 900,
                            ._depth = 1,
                            ._level = 1,
                            ._sampleCounts = 1,
                            ._format = VK_FORMAT_R8G8B8A8_UNORM });
                passData._input2 =
                    builder.allocate<RenderGraph::FrameGraphTexture>(
                        "Sample Pass 1 Resource Input 2",
                        RenderGraph::FrameGraphTexture::Descriptor{
                            ._width = 1200,
                            ._height = 900,
                            ._depth = 1,
                            ._level = 1,
                            ._sampleCounts = 1,
                            ._format = VK_FORMAT_R8G8B8A8_UNORM });
                passData._output1 =
                    builder.allocate<RenderGraph::FrameGraphTexture>(
                        "Sample Pass 1 Resource Output 1",
                        RenderGraph::FrameGraphTexture::Descriptor{
                            ._width = 1200,
                            ._height = 900,
                            ._depth = 1,
                            ._level = 1,
                            ._sampleCounts = 1,
                            ._format = VK_FORMAT_R8G8B8A8_UNORM });
                passData._input1 = builder.read<RenderGraph::FrameGraphTexture>(
                    passData._input1, TextureUsage::Sampled);
                passData._input2 = builder.read<RenderGraph::FrameGraphTexture>(
                    passData._input2, TextureUsage::Sampled);
                passData._output1 =
                    builder.write<RenderGraph::FrameGraphTexture>(
                        passData._output1, TextureUsage::RenderTarget);
            },
            [&](const RenderGraph::FrameGraphResources*,
                SamplePassData1& passData,
                CommandStream*) { passData._isExecuted = true; });

    struct SamplePassData2
    {
        RenderGraph::ResourceHandle _input1;
        RenderGraph::ResourceHandle _output1;
        bool _isExecuted = false;
    };

    const SamplePassData2& samplePass2 =
        frameGraph.addCallbackPass<SamplePassData2>(
            "Sample Pass 2 (Unreferenced)",
            [&](RenderGraph::FrameGraphBuilder& builder,
                SamplePassData2& passData) {
                passData._input1 =
                    builder.allocate<RenderGraph::FrameGraphTexture>(
                        "Sample Pass 2 Resource Input 1 (Unreferenced)",
                        RenderGraph::FrameGraphTexture::Descriptor{
                            ._width = 1200,
                            ._height = 900,
                            ._depth = 1,
                            ._level = 1,
                            ._sampleCounts = 1,
                            ._format = VK_FORMAT_R8G8B8A8_UNORM });
                passData._output1 =
                    builder.allocate<RenderGraph::FrameGraphTexture>(
                        "Sample Pass 2 Resource Output 1 (Unreferenced)",
                        RenderGraph::FrameGraphTexture::Descriptor{
                            ._width = 1200,
                            ._height = 900,
                            ._depth = 1,
                            ._level = 1,
                            ._sampleCounts = 1,
                            ._format = VK_FORMAT_R8G8B8A8_UNORM });
                passData._input1 = builder.read<RenderGraph::FrameGraphTexture>(
                    passData._input1, TextureUsage::Sampled);
                passData._output1 =
                    builder.write<RenderGraph::FrameGraphTexture>(
                        passData._output1, TextureUsage::RenderTarget);
            },
            [&](const RenderGraph::FrameGraphResources*, SamplePassData2& passData,
                CommandStream*) { passData._isExecuted = true; });

    struct SamplePassData3
    {
        RenderGraph::ResourceHandle _input1;
        RenderGraph::ResourceHandle _output1;
        bool _isExecuted = false;
    };

    const SamplePassData3& samplePass3 =
        frameGraph.addCallbackPass<SamplePassData3>(
            "Sample Pass 3",
            [&](RenderGraph::FrameGraphBuilder& builder,
                SamplePassData3& passData) {
                passData._input1 = builder.read<RenderGraph::FrameGraphTexture>(
                    samplePass1._output1, TextureUsage::Sampled);
                builder.write<RenderGraph::FrameGraphTexture>(
                    blackBoard.getHandle("BackBuffer"),
                    TextureUsage::RenderTarget);
            },
            [&](const RenderGraph::FrameGraphResources*,
                SamplePassData3& passData,
                CommandStream*) { passData._isExecuted = true; });

    const bool compileResult = frameGraph.compile();
    CHECK_EQ(compileResult, true);

    frameGraph.execute();

    CHECK_EQ(samplePass1._isExecuted, true);
    CHECK_EQ(samplePass2._isExecuted, false);
    CHECK_EQ(samplePass3._isExecuted, true);
}