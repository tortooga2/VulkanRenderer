#include <iostream>
#include <vector>
#include <optional>
#include <set>
#include <string>
#include <cstdint>
#include <limits>
#include <algorithm>

#include <vulkan/vulkan.hpp>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "fileManagment.hpp"

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

    void CreateLogicalDevice(VkPhysicalDevice &physicalDevice, VkDevice &device, VkQueue &graphicsQueue, VkQueue &presentQueue, VkSurfaceKHR &surface);

    void CreateSurface(VkInstance &instance, GLFWwindow *&window, VkSurfaceKHR &surface);

    void CreateSwapChain(GLFWwindow *&window, VkPhysicalDevice &physicalDevice, VkDevice &device, VkSurfaceKHR &surface, VkSwapchainKHR &swapChain, std::vector<VkImage> &swapChainImages, VkExtent2D &swapChainExtent, VkFormat &swapChainImageFormat);

    void CreateImageViews(VkDevice &device, std::vector<VkImageView> &swapChainImageViews, std::vector<VkImage> &swapChainImages, VkFormat &swapChainImageFormat);

    void CreateRenderPass(VkDevice &device, VkFormat &swapChainImageFormat, VkRenderPass &renderPass);

    void CreateGraphicsPipeline(VkDevice &device, VkExtent2D &swapChainExtent, VkPipelineLayout &pipelineLayout, VkRenderPass &renderPass, VkPipeline &graphicsPipeline);

    void CreateFramebuffers(VkDevice &device, std::vector<VkFramebuffer> &swapChainFramebuffers, VkRenderPass &renderPass, std::vector<VkImageView> &swapChainImageViews, VkExtent2D &swapChainExtent);

    void CreateCommandPool(VkDevice &device, VkCommandPool &commandPool, QueueFamilyIndices &indices);

    void CreateCommandBuffers(VkDevice &device, VkCommandPool &commandPool, VkCommandBuffer &commandBuffer, VkRenderPass &renderPass, std::vector<VkFramebuffer> &swapChainFramebuffers, VkExtent2D &swapChainExtent, VkPipeline &graphicsPipeline, VkPipelineLayout &pipelineLayout);

    void CreateSemaphores(VkDevice &device, VkSemaphore &semephore);

    void CreateFences(VkDevice &device, VkFence &fence);

    void GetExtensions(uint32_t &extensionCount, std::vector<const char *> &Extensions);

    bool CheckValidationLayerSupport();

    bool IsDeviceSupported(VkPhysicalDevice &device, VkSurfaceKHR &surface);

    bool CheckDeviceExtensionSupport(VkPhysicalDevice &device);

    void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks *pAllocator);

    void SetupDebugMessenger(VkInstance &instance, VkDebugUtilsMessengerEXT &debugMessenger);

    void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &createInfo);

    VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo, const VkAllocationCallbacks *pAllocator, VkDebugUtilsMessengerEXT *pDebugMessenger);

    QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR &surface);

    SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR &surface);

    VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats);

    VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR> &availablePresentModes);

    VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities, GLFWwindow *&window);

    VkShaderModule CreateShaderModule(const std::vector<char> &code, VkDevice &device);

    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
        void *pUserData);
}