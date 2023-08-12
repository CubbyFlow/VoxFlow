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

    FrameGraph::FrameGraph frameGraph;
    FrameGraph::BlackBoard& blackBoard = frameGraph.getBlackBoard();

    {
        FrameGraph::ResourceHandle backBuffer = frameGraph.importRenderTarget(
            "BackBuffer",
            FrameGraph::FrameGraphTexture::Descriptor{
                ._width = 1200,
                ._height = 900,
                ._depth = 1,
                ._level = 1,
                ._sampleCounts = 1,
                ._format = VK_FORMAT_R8G8B8A8_UNORM },
            nullptr);

        blackBoard["BackBuffer"] = backBuffer;
    }

    struct SamplePassData1
    {
        FrameGraph::ResourceHandle _input1;
        FrameGraph::ResourceHandle _input2;
        FrameGraph::ResourceHandle _output1;
        bool _isExecuted = false;
    };

    const SamplePassData1& samplePass1 =
        frameGraph.addCallbackPass<SamplePassData1>(
            "Sample Pass 1",
            [&](FrameGraph::FrameGraphBuilder& builder,
                SamplePassData1& passData) {
                passData._input1 =
                    builder.allocate<FrameGraph::FrameGraphTexture>(
                        "Sample Pass 1 Resource Input 1",
                        FrameGraph::FrameGraphTexture::Descriptor{
                            ._width = 1200,
                            ._height = 900,
                            ._depth = 1,
                            ._level = 1,
                            ._sampleCounts = 1,
                            ._format = VK_FORMAT_R8G8B8A8_UNORM });
                passData._input2 =
                    builder.allocate<FrameGraph::FrameGraphTexture>(
                        "Sample Pass 1 Resource Input 2",
                        FrameGraph::FrameGraphTexture::Descriptor{
                            ._width = 1200,
                            ._height = 900,
                            ._depth = 1,
                            ._level = 1,
                            ._sampleCounts = 1,
                            ._format = VK_FORMAT_R8G8B8A8_UNORM });
                passData._output1 =
                    builder.allocate<FrameGraph::FrameGraphTexture>(
                        "Sample Pass 1 Resource Output 1",
                        FrameGraph::FrameGraphTexture::Descriptor{
                            ._width = 1200,
                            ._height = 900,
                            ._depth = 1,
                            ._level = 1,
                            ._sampleCounts = 1,
                            ._format = VK_FORMAT_R8G8B8A8_UNORM });
                passData._input1 = builder.read(passData._input1);
                passData._input2 = builder.read(passData._input2);
                passData._output1 = builder.write(passData._output1);
            },
            [&](const FrameGraph::FrameGraphResources*,
                SamplePassData1& passData,
                CommandStream*) { passData._isExecuted = true; });

    struct SamplePassData2
    {
        FrameGraph::ResourceHandle _input1;
        FrameGraph::ResourceHandle _output1;
        bool _isExecuted = false;
    };

    const SamplePassData2& samplePass2 =
        frameGraph.addCallbackPass<SamplePassData2>(
            "Sample Pass 2 (Unreferenced)",
            [&](FrameGraph::FrameGraphBuilder& builder,
                SamplePassData2& passData) {
                passData._input1 =
                    builder.allocate<FrameGraph::FrameGraphTexture>(
                        "Sample Pass 2 Resource Input 1 (Unreferenced)",
                        FrameGraph::FrameGraphTexture::Descriptor{
                            ._width = 1200,
                            ._height = 900,
                            ._depth = 1,
                            ._level = 1,
                            ._sampleCounts = 1,
                            ._format = VK_FORMAT_R8G8B8A8_UNORM });
                passData._output1 =
                    builder.allocate<FrameGraph::FrameGraphTexture>(
                        "Sample Pass 2 Resource Output 1 (Unreferenced)",
                        FrameGraph::FrameGraphTexture::Descriptor{
                            ._width = 1200,
                            ._height = 900,
                            ._depth = 1,
                            ._level = 1,
                            ._sampleCounts = 1,
                            ._format = VK_FORMAT_R8G8B8A8_UNORM });
                passData._input1 = builder.read(passData._input1);
                passData._output1 = builder.write(passData._output1);
            },
            [&](const FrameGraph::FrameGraphResources*, SamplePassData2& passData,
                CommandStream*) { passData._isExecuted = true; });

    struct SamplePassData3
    {
        FrameGraph::ResourceHandle _input1;
        FrameGraph::ResourceHandle _output1;
        bool _isExecuted = false;
    };

    const SamplePassData3& samplePass3 =
        frameGraph.addCallbackPass<SamplePassData3>(
            "Sample Pass 3",
            [&](FrameGraph::FrameGraphBuilder& builder,
                SamplePassData3& passData) {
                passData._input1 = builder.read(samplePass1._output1);
                builder.write(blackBoard.getHandle("BackBuffer"));
            },
            [&](const FrameGraph::FrameGraphResources*,
                SamplePassData3& passData,
                CommandStream*) { passData._isExecuted = true; });

    const bool compileResult = frameGraph.compile();
    CHECK_EQ(compileResult, true);

    frameGraph.execute();

    CHECK_EQ(samplePass1._isExecuted, true);
    CHECK_EQ(samplePass2._isExecuted, false);
    CHECK_EQ(samplePass3._isExecuted, true);
}