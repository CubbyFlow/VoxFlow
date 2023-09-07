#include <iostream>
#include <VoxFlow/Editor/VoxEditor.hpp>

int main(int argc, char* argv[])
{
    cxxopts::Options options(
        "VoxEditor", "CubbyFlow(Voxel-based fluid simulation engine) editor");

    options.add_options()("d,debug", "Enable vulkan validation layer",
                          cxxopts::value<bool>()->default_value("false"));

    VoxFlow::VoxEditor editor(options.parse(argc, argv));

    editor.runEditorLoop();

    return 0;
}