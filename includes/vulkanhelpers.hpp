#include <iostream>
#include <vector>
#include <optional>
#include <set>
#include <string>

#include <vulkan/vulkan.hpp>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace VKHelpers
{

#ifdef NDEBUG
    const bool enableValidationLayers = false;
#else
    const bool enableValidationLayers = true;
#endif

    struct QueueFamilyIndices
    {
        std::optional<uint32_t> graphicsFamily;
        std::optional<uint32_t> presentFamily;

        bool isComplete()
        {
            return graphicsFamily.has_value() && presentFamily.has_value();
        }
    };

    struct SwapChainSupportDetails
    {
        VkSurfaceCapabilitiesKHR capabilities;
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR> presentModes;
    };

    const std::vector<const char *> deviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

    const std::vector<const char *> validationLayers = {"VK_LAYER_KHRONOS_validation"};

    void CreateWindow(GLFWwindow *&window, const int width, const int height, const char *title);

    void CreateVulkanInstance(VkInstance &instance);

    void PickPhysicalDevice(VkInstance &instance, VkPhysicalDevice &physicalDevice, VkSurfaceKHR &surface);

    QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR &surface);

    void CreateLogicalDevice(VkPhysicalDevice &physicalDevice, VkDevice &device, VkQueue &graphicsQueue, VkQueue &presentQueue, VkSurfaceKHR &surface);

    void CreateSurface(VkInstance &instance, GLFWwindow *&window, VkSurfaceKHR &surface);

    void GetExtensions(uint32_t &extensionCount, std::vector<const char *> &Extensions);

    bool CheckValidationLayerSupport();

    bool IsDeviceSupported(VkPhysicalDevice &device, VkSurfaceKHR &surface);

    bool CheckDeviceExtensionSupport(VkPhysicalDevice &device);

    VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo, const VkAllocationCallbacks *pAllocator, VkDebugUtilsMessengerEXT *pDebugMessenger);

    void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks *pAllocator);

    void SetupDebugMessenger(VkInstance &instance, VkDebugUtilsMessengerEXT &debugMessenger);

    void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &createInfo);

    SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR &surface);

    VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats);

    VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR> &availablePresentModes);

    VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities, GLFWwindow *&window, int width, int height);

    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
        void *pUserData);
}