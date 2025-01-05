#include "vulkanhelpers.hpp"

void VKHelpers::CreateWindow(GLFWwindow *&window, const int width, const int height, const char *title)
{
    if (!glfwInit())
    {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        exit(EXIT_FAILURE);
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    window = glfwCreateWindow(width, height, title, nullptr, nullptr);
    if (!window)
    {
        std::cerr << "Failed to create window" << std::endl;
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
}

void VKHelpers::CreateVulkanInstance(VkInstance &instance)
{
    VkApplicationInfo appInfo = {};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Hello Triangle";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_2;

    VkInstanceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

    uint32_t glfwExtensionCount = 0;
    std::vector<const char *> Extensions;
    GetExtensions(glfwExtensionCount, Extensions);

    createInfo.enabledExtensionCount = glfwExtensionCount;
    createInfo.ppEnabledExtensionNames = Extensions.data();

#ifdef __APPLE__
    createInfo.flags |= VK_INSTANCE_CREATE_PORTABILITY_SUBSET_BIT_KHR;
#endif

    createInfo.enabledLayerCount = 0;

    if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create instance");
    }
    else
    {
        std::cout << "Vulkan instance created successfully" << std::endl;
    }
}

void VKHelpers::GetExtensions(uint32_t &ExtensionCount, std::vector<const char *> &Extensions)
{

    const char **glfwExtensionsArray = glfwGetRequiredInstanceExtensions(&ExtensionCount);
    for (uint32_t i = 0; i < ExtensionCount; i++)
    {
        std::cout << "GLFW Extension: " << glfwExtensionsArray[i] << std::endl;
        Extensions.push_back(glfwExtensionsArray[i]);
    }

#ifdef __APPLE__
    glfwExtensions.push_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
#endif
}