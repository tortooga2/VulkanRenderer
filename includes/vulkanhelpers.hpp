#include <iostream>
#include <vector>

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

    const std::vector<const char *> validationLayers = {"VK_LAYER_KHRONOS_validation"};

    void CreateWindow(GLFWwindow *&window, const int width, const int height, const char *title);

    void CreateVulkanInstance(VkInstance &instance);

    void GetExtensions(uint32_t &extensionCount, std::vector<const char *> &Extensions);

    bool CheckValidationLayerSupport();

    VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo, const VkAllocationCallbacks *pAllocator, VkDebugUtilsMessengerEXT *pDebugMessenger);

    void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks *pAllocator);

    void SetupDebugMessenger(VkInstance &instance, VkDebugUtilsMessengerEXT &debugMessenger);

    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
        void *pUserData);
}