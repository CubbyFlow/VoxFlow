// Author : snowapril

#include <glfw/glfw3.h>
#include <VoxFlow/Editor/VoxEditor.hpp>
#include <VoxFlow/Editor/VoxEngine.hpp>
#include <chrono>

namespace VoxFlow
{

VoxEditor::VoxEditor()
{
    _voxEngine = new VoxEngine();
}

VoxEditor::~VoxEditor()
{
    if (_voxEngine != nullptr)
        delete _voxEngine;
}

void VoxEditor::runEditorLoop()
{
    using namespace std::chrono;
    bool exit = false;

    // system_clock::time_point previousTime = system_clock::now();

    while (exit == false)
    {
        // system_clock::time_point currentTime = system_clock::now();
        // const uint64_t elapsed =
        //     duration_cast<milliseconds>(currentTime - previousTime).count();
        // previousTime = currentTime;

        processInput();

        preUpdateFrame();
        updateFrame();
        renderFrame();
        postRenderFrame();
    }
}

void VoxEditor::processInput()
{
    glfwPollEvents();
}

void VoxEditor::preUpdateFrame()
{
}

void VoxEditor::updateFrame()
{
}

void VoxEditor::renderFrame()
{
}

void VoxEditor::postRenderFrame()
{
}

}  // namespace VoxFlow