#include "vulkanhelpers.hpp"
#include "fileManagment.hpp"

namespace gfx
{

    struct VulkanInstance
    {
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
        std::vector<VkFramebuffer> swapChainFramebuffers;

        VkRenderPass renderPass;
        VkPipelineLayout pipelineLayout;
        VkPipeline graphicsPipeline;

        VkCommandPool commandPool;
        std::vector<VkCommandBuffer> commandBuffers;

        std::vector<VkSemaphore> imageAvailableSemaphores;
        std::vector<VkSemaphore> renderFinishedSemaphores;
        std::vector<VkFence> inFlightFences;

        uint32_t currentFrame = 0;
        const int MAX_FRAMES_IN_FLIGHT = 2;

        void initVulkan(GLFWwindow *&window)
        {
            VKHelpers::CreateVulkanInstance(instance);
            VKHelpers::SetupDebugMessenger(instance, debugMessenger);
            VKHelpers::CreateSurface(instance, window, surface);
            VKHelpers::PickPhysicalDevice(instance, physicalDevice, surface);
            VKHelpers::CreateLogicalDevice(physicalDevice, device, graphicsQueue, presentQueue, surface);
            VKHelpers::CreateSwapChain(window, physicalDevice, device, surface, swapChain, swapChainImages, swapChainExtent, swapChainImageFormat);
            VKHelpers::CreateImageViews(device, swapChainImageViews, swapChainImages, swapChainImageFormat);
            VKHelpers::CreateRenderPass(device, swapChainImageFormat, renderPass);
            VKHelpers::CreateGraphicsPipeline(device, swapChainExtent, pipelineLayout, renderPass, graphicsPipeline);
            VKHelpers::CreateFramebuffers(device, swapChainFramebuffers, renderPass, swapChainImageViews, swapChainExtent);
            auto queueFam = VKHelpers::FindQueueFamilies(physicalDevice, surface);
            VKHelpers::CreateCommandPool(device, commandPool, queueFam);

            VKHelpers::CreateCommandMultipleBuffers(device, commandPool, commandBuffers, MAX_FRAMES_IN_FLIGHT, renderPass, swapChainFramebuffers, swapChainExtent, graphicsPipeline, pipelineLayout);
            imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
            renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
            inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);
            for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
            {
                VKHelpers::CreateSemaphores(device, imageAvailableSemaphores[i]);
                VKHelpers::CreateSemaphores(device, renderFinishedSemaphores[i]);
                VKHelpers::CreateFences(device, inFlightFences[i]);
            }
        };

        void cleanupSwapChain()
        {
            for (auto framebuffer : swapChainFramebuffers)
            {
                vkDestroyFramebuffer(device, framebuffer, nullptr);
            }
            for (auto swapChainImageView : swapChainImageViews)
            {
                vkDestroyImageView(device, swapChainImageView, nullptr);
            }
            vkDestroySwapchainKHR(device, swapChain, nullptr);
        }

        void cleanup()
        {
            cleanupSwapChain();
            for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
            {
                vkDestroySemaphore(device, renderFinishedSemaphores[i], nullptr);
                vkDestroySemaphore(device, imageAvailableSemaphores[i], nullptr);
                vkDestroyFence(device, inFlightFences[i], nullptr);
            }
            vkDestroyCommandPool(device, commandPool, nullptr);

            vkDestroyPipeline(device, graphicsPipeline, nullptr);
            vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
            vkDestroyRenderPass(device, renderPass, nullptr);

            if (VKHelpers::enableValidationLayers)
            {
                VKHelpers::DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
            }

            vkDestroyDevice(device, nullptr);
            vkDestroySurfaceKHR(instance, surface, nullptr);
            vkDestroyInstance(instance, nullptr);
        }

        void RecreateSwapChain(GLFWwindow *&window)
        {
            vkDeviceWaitIdle(device);

            cleanupSwapChain();

            VKHelpers::CreateSwapChain(window, physicalDevice, device, surface, swapChain, swapChainImages, swapChainExtent, swapChainImageFormat);
            VKHelpers::CreateImageViews(device, swapChainImageViews, swapChainImages, swapChainImageFormat);
            VKHelpers::CreateFramebuffers(device, swapChainFramebuffers, renderPass, swapChainImageViews, swapChainExtent);
        }
    };
}