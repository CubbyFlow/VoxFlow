// Author : snowapril

#include <doctest/doctest_proxy.hpp>

TEST_CASE("Vulkan Instance Initialization")
{
    VoxFlow::Instance instance(gVulkanContext);

    CHECK_NE(instance.get(), VK_NULL_HANDLE);
}