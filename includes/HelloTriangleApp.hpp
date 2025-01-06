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
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    VkDevice device;
    VkDebugUtilsMessengerEXT debugMessenger;
    VkQueue graphicsQueue;

    void initWindow()
    {
        VKHelpers::CreateWindow(window, WIDTH, HEIGHT, "New Window");
    };
    void initVulkan()
    {
        VKHelpers::CreateVulkanInstance(instance);
        VKHelpers::SetupDebugMessenger(instance, debugMessenger);
        VKHelpers::PickPhysicalDevice(instance, physicalDevice);
        VKHelpers::CreateLogicalDevice(physicalDevice, device);
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
        if (VKHelpers::enableValidationLayers)
        {
            VKHelpers::DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
        }
        vkDestroyDevice(device, nullptr);
        vkDestroyInstance(instance, nullptr);

        glfwDestroyWindow(window);
        glfwTerminate();
    };
};