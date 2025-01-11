#include "vulkanhelpers.hpp"
#include "fileManagment.hpp"

#ifdef NDEBUG
const char *path = "./Shaders/";
#else
const char *path = "./Shaders/compiled/";
#endif

class MainApp
{
public:
    MainApp(int width, int height) : WIDTH(width), HEIGHT(height) {};

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
    VkQueue presentQueue;
    VkSurfaceKHR surface;
    VkSwapchainKHR swapChain;
    std::vector<VkImage> swapChainImages;
    VkExtent2D swapChainExtent;
    VkFormat swapChainImageFormat;
    std::vector<VkImageView> swapChainImageViews;

    void initWindow()
    {
        VKHelpers::CreateWindow(window, WIDTH, HEIGHT, "Nature Engine");
    };
    void initVulkan()
    {
        VKHelpers::CreateVulkanInstance(instance);
        VKHelpers::SetupDebugMessenger(instance, debugMessenger);
        VKHelpers::CreateSurface(instance, window, surface);
        VKHelpers::PickPhysicalDevice(instance, physicalDevice, surface);
        VKHelpers::CreateLogicalDevice(physicalDevice, device, graphicsQueue, presentQueue, surface);
        VKHelpers::CreateSwapChain(window, physicalDevice, device, surface, swapChain, swapChainImages, swapChainExtent, swapChainImageFormat);
        VKHelpers::CreateImageViews(device, swapChainImageViews, swapChainImages, swapChainImageFormat);
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
        for (auto imageView : swapChainImageViews)
        {
            vkDestroyImageView(device, imageView, nullptr);
        }
        vkDestroySwapchainKHR(device, swapChain, nullptr);

        if (VKHelpers::enableValidationLayers)
        {
            VKHelpers::DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
        }

        vkDestroyDevice(device, nullptr);
        vkDestroySurfaceKHR(instance, surface, nullptr);
        vkDestroyInstance(instance, nullptr);

        glfwDestroyWindow(window);
        glfwTerminate();
    };
};