// Author : snowapril

#include <VoxFlow/Editor/RenderPass/UIRenderPass.hpp>
#include <VoxFlow/Core/Devices/LogicalDevice.hpp>
#include <VoxFlow/Core/Devices/PhysicalDevice.hpp>
#include <VoxFlow/Core/Devices/Instance.hpp>
#include <VoxFlow/Core/Devices/SwapChain.hpp>
#include <VoxFlow/Editor/Utils/ImGuiUtils.hpp>
#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_vulkan.h>

namespace VoxFlow
{
UIRenderPass::UIRenderPass(LogicalDevice* logicalDevice, SwapChain* swapChain) : _logicalDevice(logicalDevice), _swapChain(swapChain)
{
}

UIRenderPass::~UIRenderPass()
{
}

bool UIRenderPass::initialize()
{
    // TODO(snowapril) : resize descriptor pool sizes
    VkDescriptorPoolSize poolSize[] = {
        { VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
        { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
        { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
        { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
        { VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
        { VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
        { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
        { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
        { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
        { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
        { VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 },
    };

    VkDescriptorPoolCreateInfo descriptorPoolInfo = {};
    descriptorPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    descriptorPoolInfo.pNext = nullptr;
    descriptorPoolInfo.poolSizeCount = sizeof(poolSize) / sizeof(VkDescriptorPoolSize);
    descriptorPoolInfo.pPoolSizes = poolSize;
    descriptorPoolInfo.maxSets = 1000;  // TODO(snowapril) : same with above TODO
    descriptorPoolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;

    VkResult result = vkCreateDescriptorPool(_logicalDevice->get(), &descriptorPoolInfo, nullptr, &_imguiDescriptorPool);
    if (result != VK_SUCCESS)
    {
        VK_ASSERT(result);
        return false;
    }

    ImGui::CreateContext();
    ImGui::StyleCinder(nullptr);
    // ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForVulkan(_swapChain->getGlfwWindow(), true);
    ImGui_ImplVulkan_InitInfo imGuiInitData = {};
    imGuiInitData.Device = _logicalDevice->get();
    imGuiInitData.DescriptorPool = _imguiDescriptorPool;
    imGuiInitData.PhysicalDevice = _logicalDevice->getPhysicalDevice()->get();
    imGuiInitData.Allocator = nullptr;
    imGuiInitData.Instance = _logicalDevice->getInstance()->get();
    imGuiInitData.QueueFamily = _graphicsQueue->getFamilyIndex();
    imGuiInitData.Queue = _graphicsQueue->getQueueHandle();
    imGuiInitData.PipelineCache = VK_NULL_HANDLE;
    imGuiInitData.CheckVkResultFn = nullptr;

    // TODO(snowapril) : minImageCount and imageCount are hard-coded now. need to fix
    imGuiInitData.MinImageCount = 10;  // TODO(snowapril) : this may cause error
    imGuiInitData.ImageCount = 10;     // TODO(snowapril) : this may cause error
    imGuiInitData.MSAASamples = VK_SAMPLE_COUNT_1_BIT;

    if (!ImGui_ImplVulkan_Init(&imGuiInitData, renderPass))
    {
        return false;
    }

    return true;
}
void UIRenderPass::updateRender(ResourceUploadContext* uploadContext)
{
}
void UIRenderPass::renderScene(RenderGraph::FrameGraph* frameGraph)
{

};
}  // namespace VoxFlow