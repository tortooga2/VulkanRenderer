#pragma once

#include "vulkanhelpers.hpp"
#include "VBAllocator.hpp"
#include "UniformManager.hpp"
#include "fileManagment.hpp"

// namespace gfx
// {

struct VulkanInstance
{
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
    uint32_t imageIndex = 0;

    VkClearValue clearColor = {0.0f, 0.0f, 0.0f, 1.0f};
    VkViewport viewport;
    VkRect2D scissor;
    // Flags
    bool FrameStarted = false;
    bool CommandBufferRecordingStarted = false;
    bool RenderPassStarted = false;

    void initVulkan(GLFWwindow *&window)
    {
        // Create Window
        this->window = window;

        // Initialize Vulkan
        VKHelpers::CreateVulkanInstance(instance);
        VKHelpers::SetupDebugMessenger(instance, debugMessenger);
        VKHelpers::CreateSurface(instance, window, surface);
        VKHelpers::PickPhysicalDevice(instance, physicalDevice, surface);
        VKHelpers::CreateLogicalDevice(physicalDevice, device, graphicsQueue, presentQueue, surface);
        VKHelpers::CreateSwapChain(window, physicalDevice, device, surface, swapChain, swapChainImages, swapChainExtent, swapChainImageFormat);
        VKHelpers::CreateImageViews(device, swapChainImageViews, swapChainImages, swapChainImageFormat);
        VKHelpers::CreateRenderPass(device, swapChainImageFormat, renderPass);

        VKHelpers::CreateFramebuffers(device, swapChainFramebuffers, renderPass, swapChainImageViews, swapChainExtent);
        auto queueFam = VKHelpers::FindQueueFamilies(physicalDevice, surface);
        VKHelpers::CreateCommandPool(device, commandPool, queueFam);

        VKHelpers::CreateCommandMultipleBuffers(device, commandPool, commandBuffers, MAX_FRAMES_IN_FLIGHT);
        imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
        renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
        inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);
        for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
        {
            VKHelpers::CreateSemaphores(device, imageAvailableSemaphores[i]);
            VKHelpers::CreateSemaphores(device, renderFinishedSemaphores[i]);
            VKHelpers::CreateFences(device, inFlightFences[i]);
        }

        SetRecordCommandBufferSettings();
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
        SetRecordCommandBufferSettings();
    }

    void BeginDrawFrame()
    {
        if (FrameStarted == true)
        {
            throw std::runtime_error("Frame already started! Nothing to Start!");
        }
        if (CommandBufferRecordingStarted == true)
        {
            throw std::runtime_error("Command buffer not stopped!");
        }
        if (RenderPassStarted == true)
        {
            throw std::runtime_error("Render pass not stopped!");
        }
        else
        {
            FrameStarted = true;
        }

        vkWaitForFences(device, 1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);

        VkResult result = vkAcquireNextImageKHR(device, swapChain, UINT64_MAX, imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);
        if (result == VK_ERROR_OUT_OF_DATE_KHR)
        {
            RecreateSwapChain(window);
            return;
        }
        else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
        {
            throw std::runtime_error("Failed to acquire swap chain image!");
        }

        vkResetFences(device, 1, &inFlightFences[currentFrame]);
    }

    void EndDrawFrame()
    {
        if (FrameStarted == false)
        {
            throw std::runtime_error("Frame not started! Nothing to Stop!");
        }
        else if (CommandBufferRecordingStarted == true)
        {
            throw std::runtime_error("Command buffer not stopped!");
        }
        else if (RenderPassStarted == true)
        {
            throw std::runtime_error("Render pass not stopped!");
        }
        else
        {
            FrameStarted = false;
        }

        VkSubmitInfo submitInfo = {};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

        VkSemaphore waitSemaphores[] = {imageAvailableSemaphores[currentFrame]};
        VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = waitSemaphores;
        submitInfo.pWaitDstStageMask = waitStages;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandBuffers[currentFrame];

        VkSemaphore signalSemaphores[] = {renderFinishedSemaphores[currentFrame]};
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = signalSemaphores;

        if (vkQueueSubmit(graphicsQueue, 1, &submitInfo, inFlightFences[currentFrame]) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to submit draw command buffer!");
        }
    }

    void PresentFinalFrame()
    {
        VkSemaphore signalSemaphores[] = {renderFinishedSemaphores[currentFrame]};

        VkPresentInfoKHR presentInfo = {};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = signalSemaphores;

        VkSwapchainKHR swapChains[] = {swapChain};
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = swapChains;
        presentInfo.pImageIndices = &imageIndex;
        presentInfo.pResults = nullptr; // Optional
        VkResult result = vkQueuePresentKHR(presentQueue, &presentInfo);
        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
        {
            RecreateSwapChain(window);
        }
        else if (result != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to present swap chain image!");
        }

        currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
    }

    void BeginCommandBuffer()
    {
        if (FrameStarted == false)
        {
            throw std::runtime_error("Frame not started! Cannot start command buffer!");
        }
        else if (CommandBufferRecordingStarted == true)
        {
            throw std::runtime_error("Command buffer already started! Nothing to Start!");
        }
        else if (RenderPassStarted == true)
        {
            throw std::runtime_error("Render pass not stopped!");
        }
        else
        {
            CommandBufferRecordingStarted = true;
        }

        VkCommandBufferBeginInfo beginInfo = {};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = 0;                  // Optional
        beginInfo.pInheritanceInfo = nullptr; // Optional

        if (vkBeginCommandBuffer(commandBuffers[currentFrame], &beginInfo) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to begin recording command buffer!");
        }
    }

    void EndCommandBuffer()
    {
        if (FrameStarted == false)
        {
            throw std::runtime_error("Frame not started! Cannot end command buffer!");
        }
        else if (CommandBufferRecordingStarted == false)
        {
            throw std::runtime_error("Command buffer already stopped! Cannot end Command Buffer!");
        }
        else if (RenderPassStarted == true)
        {
            throw std::runtime_error("Render pass not stopped! Cannot end Command Buffer!");
        }
        else
        {
            CommandBufferRecordingStarted = false;
        }

        if (vkEndCommandBuffer(commandBuffers[currentFrame]) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to record command buffer!");
        }
    }

    void BeginRenderPass()
    {
        if (FrameStarted == false)
        {
            throw std::runtime_error("Frame not started! Cannot begin render pass!");
        }
        else if (CommandBufferRecordingStarted == false)
        {
            throw std::runtime_error("Command buffer not started! Cannot begin render pass!");
        }
        else if (RenderPassStarted == true)
        {
            throw std::runtime_error("Render pass not stopped! Cannot start render pass!");
        }
        else
        {
            RenderPassStarted = true;
        }

        VkRenderPassBeginInfo renderPassInfo = {};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = renderPass;
        renderPassInfo.framebuffer = swapChainFramebuffers[imageIndex]; // We only have one framebuffer
        renderPassInfo.renderArea.offset = {0, 0};
        renderPassInfo.renderArea.extent = swapChainExtent;
        renderPassInfo.clearValueCount = 1;
        renderPassInfo.pClearValues = &clearColor;

        vkCmdBeginRenderPass(commandBuffers[currentFrame], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
    }
    // TODO:: Implement this function only once we have created our framebuffer object.
    // void BeginRenderPass(VkRenderPass renderPass)
    // {
    //     if (FrameStarted == false)
    //     {
    //         throw std::runtime_error("Frame not started! Cannot begin render pass!");
    //     }
    //     else if (CommandBufferRecordingStarted == false)
    //     {
    //         throw std::runtime_error("Command buffer not started! Cannot begin render pass!");
    //     }
    //     else if (RenderPassStarted == true)
    //     {
    //         throw std::runtime_error("Render pass not stopped! Cannot start render pass!");
    //     }
    //     else
    //     {
    //         RenderPassStarted = true;
    //     }

    //     VkRenderPassBeginInfo renderPassInfo = {};
    //     renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    //     renderPassInfo.renderPass = renderPass;
    //     renderPassInfo.framebuffer = swapChainFramebuffers[imageIndex]; // We only have one framebuffer
    //     renderPassInfo.renderArea.offset = {0, 0};
    //     renderPassInfo.renderArea.extent = swapChainExtent;
    //     renderPassInfo.clearValueCount = 1;
    //     renderPassInfo.pClearValues = &clearColor;

    //     vkCmdBeginRenderPass(commandBuffers[currentFrame], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
    // }

    void EndRenderPass()
    {
        if (FrameStarted == false)
        {
            throw std::runtime_error("Frame not started! Cannot end render pass!");
        }
        else if (CommandBufferRecordingStarted == false)
        {
            throw std::runtime_error("Command buffer not started! Cannot end render pass!");
        }
        else if (RenderPassStarted == false)
        {
            throw std::runtime_error("Render pass already stopped! Cannot end render pass!");
        }
        else
        {
            RenderPassStarted = false;
        }

        vkCmdEndRenderPass(commandBuffers[currentFrame]);
    }

    void BindGraphicsPipeline(VkPipeline &pipeline)
    {
        if (FrameStarted == false)
        {
            throw std::runtime_error("Frame not started! Cannot bind pipeline!");
        }
        else if (CommandBufferRecordingStarted == false)
        {
            throw std::runtime_error("Command buffer not started! Cannot bind pipeline!");
        }
        else if (RenderPassStarted == false)
        {
            throw std::runtime_error("Render pass not started! Cannot bind pipeline!");
        }

        vkCmdBindPipeline(commandBuffers[currentFrame], VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
    }

    void ResetCommandButter()
    {
        vkResetCommandBuffer(commandBuffers[currentFrame], 0);
    }

    void SetRecordCommandBufferSettings()
    {
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = (float)swapChainExtent.width;
        viewport.height = (float)swapChainExtent.height;
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;

        scissor.offset = {0, 0};
        scissor.extent = swapChainExtent;

        
        // clearColor = {1.0f, 0.0f, 0.0f, 1.0f};
    }

    void ChangeViewportSize(uint32_t width, uint32_t height)
    {
        viewport.width = (float)width;
        viewport.height = (float)height;
    }

    void ResetViewport()
    {
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = (float)swapChainExtent.width;
        viewport.height = (float)swapChainExtent.height;
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
    }

    void SetViewportandScissor(VkCommandBuffer &commandBuffer)
    {
        vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
        vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
    }

    void ChangeBackgroundColor(float r, float g, float b, float a)
    {
        if (RenderPassStarted == true)
        {
            std::cout << "Render pass already started! Cannot change background color!" << std::endl;
            return;
        }
        clearColor = {r, g, b, a};
    }

    static VulkanInstance &getInstance()
    {
        static VulkanInstance instance;
        return instance;
    }

private:
    VulkanInstance() = default; // Private constructor
    VulkanInstance(const VulkanInstance &) = delete;
    void operator=(const VulkanInstance &) = delete;
};

// } // namespace gfx
