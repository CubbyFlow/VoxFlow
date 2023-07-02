// Author : snowapril

#include <VoxFlow/Core/FrameGraph/FrameGraph.hpp>
#include <VoxFlow/Core/FrameGraph/FrameGraphTexture.hpp>
#include <VoxFlow/Core/Graphics/Commands/CommandExecutor.hpp>
#include <sstream>
#include <string>
#include "../../UnitTestUtils.hpp"

TEST_CASE("FrameGraph")
{
    using namespace VoxFlow;

    FrameGraph::FrameGraph frameGraph;

    struct SamplePassData1
    {
        FrameGraph::ResourceHandle _input1;
        FrameGraph::ResourceHandle _input2;
        FrameGraph::ResourceHandle _output1;
        bool _isExecuted = false;
    };

    const SamplePassData1& samplePass1 = frameGraph.addCallbackPass<
        SamplePassData1>(
        "Sample Pass 1",
        [&](FrameGraph::FrameGraphBuilder& builder, SamplePassData1& passData) {
            passData._input1 = builder.allocate<FrameGraph::FrameGraphTexture>(
                "Resource Input 1", FrameGraph::FrameGraphTexture::Descriptor{
                                        ._width = 1200,
                                        ._height = 900,
                                        ._depth = 1,
                                        ._level = 1,
                                        ._sampleCounts = 1,
                                        ._format = VK_FORMAT_R8G8B8A8_UNORM });
            passData._input1 = builder.read(passData._input1);
            passData._input2 = builder.allocate<FrameGraph::FrameGraphTexture>(
                "Resource Input 2", FrameGraph::FrameGraphTexture::Descriptor{
                                        ._width = 1200,
                                        ._height = 900,
                                        ._depth = 1,
                                        ._level = 1,
                                        ._sampleCounts = 1,
                                        ._format = VK_FORMAT_R8G8B8A8_UNORM });
            passData._input2 = builder.read(passData._input2);
            passData._output1 = builder.allocate<FrameGraph::FrameGraphTexture>(
                "Resource Output 1", FrameGraph::FrameGraphTexture::Descriptor{
                                         ._width = 1200,
                                         ._height = 900,
                                         ._depth = 1,
                                         ._level = 1,
                                         ._sampleCounts = 1,
                                         ._format = VK_FORMAT_R8G8B8A8_UNORM });
            passData._output1 = builder.write(passData._output1);
        },
        [&](FrameGraph::FrameGraph*, SamplePassData1& passData,
            CommandExecutorBase*) { passData._isExecuted = true; });

    struct SamplePassData2
    {
        FrameGraph::ResourceHandle _input1;
        FrameGraph::ResourceHandle _output1;
        bool _isExecuted = false;
    };

    const SamplePassData2& samplePass2 =
        frameGraph.addCallbackPass<SamplePassData2>(
            "Sample Pass 2",
            [&](FrameGraph::FrameGraphBuilder& builder,
                SamplePassData2& passData) {
                passData._input1 = builder.read(samplePass1._output1);
            },
            [&](FrameGraph::FrameGraph*, SamplePassData2& passData,
                CommandExecutorBase*) { passData._isExecuted = true; });

    const bool compileResult = frameGraph.compile();
    CHECK_EQ(compileResult, true);

    frameGraph.execute();
    CHECK_EQ(samplePass1._isExecuted, true);
    CHECK_EQ(samplePass2._isExecuted, true);

    std::ostringstream osstr;
    frameGraph.dumpGraphViz(osstr);

    std::string dumpedGraphViz(osstr.str());
    std::string expectedGraphVizText = "";
    CHECK_EQ(dumpedGraphViz, expectedGraphVizText);
}