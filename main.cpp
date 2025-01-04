#include <iostream>

#include <vulkan/vulkan.h>
#include <glfw/glfw3.h>

#define SCR_WIDTH 800
#define SCR_HEIGHT 600

bool isVulkanAvailable()
{
    VkInstance instance;
    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    return vkCreateInstance(&createInfo, nullptr, &instance) == VK_SUCCESS;
}

int main()
{

    glfwInit();

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "OpenGLProject", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    if (isVulkanAvailable())
    {
        std::cout << "Vulkan is available on this system!" << std::endl;
    }
    else
    {
        std::cout << "Vulkan is not available." << std::endl;
    }

    glfwWindowHint(GLFW_DEPTH_BITS, 24); // Request a depth buffer with 24 bits

    while (!glfwWindowShouldClose(window))
    {

        glfwPollEvents();
    }

    glfwTerminate();

    return 0;
}