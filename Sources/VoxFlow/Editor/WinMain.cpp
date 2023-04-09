#include <iostream>
#include <VoxFlow/Editor/VoxEditor.hpp>

int main(int argc, char* argv[])
{
    (void)argc;
    (void)argv;

    std::cout << "Hello VoxFlow Editor" << std::endl;

    VoxFlow::VoxEditor editor;

    editor.runEditorLoop();

    return 0;
}