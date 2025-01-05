#include <iostream>
#include <vector>

#include <vulkan/vulkan.hpp>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace VKHelpers
{
    void CreateWindow(GLFWwindow *&window, const int width, const int height, const char *title);

    void CreateVulkanInstance(VkInstance &instance);

    void GetExtensions(uint32_t &extensionCount, std::vector<const char *> &Extensions);

}