#include "vulkanhelpers.hpp"

class HelloTriangleApp
{
public:
    HelloTriangleApp(int width, int height) : WIDTH(width), HEIGHT(height) {};

    void run()
    {
        initWindow();
        initVulkan();
        mainLoop();
        cleanup();
    };

private:
    const int WIDTH = 800;
    const int HEIGHT = 600;

    GLFWwindow *window;
    VkInstance instance;

    void initWindow()
    {
        VKHelpers::CreateWindow(window, WIDTH, HEIGHT, "New Window");
    };
    void initVulkan()
    {
        VKHelpers::CreateVulkanInstance(instance);
    };
    void mainLoop()
    {
        while (!glfwWindowShouldClose(window))
        {
            glfwPollEvents();
        }
    };
    void cleanup()
    {
        glfwDestroyWindow(window);
        glfwTerminate();
    };
};