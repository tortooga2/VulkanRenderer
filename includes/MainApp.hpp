#include "fileManagment.hpp"

#include "Graphics.hpp"

#ifdef NDEBUG
const char *path = "./Shaders/";
#else
const char *path = "./Shaders/compiled/";
#endif

std::vector<VKHelpers::Vertex> vertices = {
    {{-1.0f, -1.0f, 0.0f}, {1.0f, 0.0f, 0.0f}},
    {{1.0f, 1.0f, 0.0f}, {1.0f, 0.0f, 0.0f}},
    {{-0.5f, 0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}}};

std::vector<VKHelpers::Vertex> vertices2 = {
    {{0.0f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}},
    {{0.5f, 0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}},
    {{-0.5f, 0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}}};

std::vector<VKHelpers::Vertex> vertices3 = {
    {{0.3f, -0.2f, 0.0f}, {1.0f, 0.0f, 0.0f}},
    {{0.8f, 0.8f, 0.0f}, {0.0f, 1.0f, 0.0f}},
    {{-0.2f, 0.8f, 0.0f}, {0.0f, 0.0f, 1.0f}}};

class MainApp
{
public:
    MainApp(int width, int height) : WIDTH(width), HEIGHT(height), vbAllocator(VulkanInstance::getInstance().device) {
                                     };

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
    // const int MAX_FRAMES_IN_FLIGHT = 2;
    // uint32_t currentFrame = 0;

    GLFWwindow *window;
    VBAllocator<VKHelpers::Vertex> vbAllocator;
    VkBuffer vertexBuffer;
    VkBuffer vertexBuffer2;
    VkBuffer vertexBuffer3;
    // VulkanInstance vkInst;

    UBOMgr UB;
    VkDescriptorSetLayout descriptorSetLayout;
    VkDescriptorSet descriptorSet1;
    VkDescriptorSet descriptorSet2;
    VkDescriptorSet descriptorSet3;

    UniformBufferObject ubo1 = {glm::vec3(1.0f, 1.0f, 0.0f), glm::vec3(0.0)};
    UniformBufferObject ubo2 = {glm::vec3(1.0f, 0.0f, 1.0f), glm::vec3(0.0)};
    UniformBufferObject ubo3 = {glm::vec3(0.0f, 1.0f, 1.0f), glm::vec3(0.0)};

    void initWindow()
    {
        VKHelpers::CreateWindow(window, WIDTH, HEIGHT, "Nature Engine");
    };
    void initVulkan()
    {
        VulkanInstance::getInstance().initVulkan(window);
        UB.CreateLayout(VulkanInstance::getInstance().device, descriptorSetLayout, sizeof(UniformBufferObject));
        UB.CreatePipelineLayout(VulkanInstance::getInstance().device, VulkanInstance::getInstance().pipelineLayout);
        VKHelpers::CreateGraphicsPipeline(VulkanInstance::getInstance().device, VulkanInstance::getInstance().swapChainExtent, VulkanInstance::getInstance().pipelineLayout, VulkanInstance::getInstance().renderPass, VulkanInstance::getInstance().graphicsPipeline);
        UB.SubscribeToLayout(descriptorSetLayout, descriptorSet1);
        UB.SubscribeToLayout(descriptorSetLayout, descriptorSet2);
        UB.SubscribeToLayout(descriptorSetLayout, descriptorSet3);
        UB.CreatePool(VulkanInstance::getInstance().device);
        UB.AllocateDescriptors(VulkanInstance::getInstance().device, VulkanInstance::getInstance().physicalDevice);
        VulkanInstance::getInstance().ChangeBackgroundColor(0.0f, 0.0f, 0.0f, 1.0f);
        UB.SetUniform(descriptorSet1, ubo1);
        UB.SetUniform(descriptorSet2, ubo2);
        UB.SetUniform(descriptorSet3, ubo3);

        vbAllocator.AddMesh(&vertices, vertexBuffer);
        vbAllocator.AddMesh(&vertices2, vertexBuffer2);
        vbAllocator.AddMesh(&vertices3, vertexBuffer3);

        vbAllocator.AllocateVertexBuffer(VulkanInstance::getInstance().physicalDevice);
    };

    void mainLoop()
    {
        float t = 0.0f;
        while (!glfwWindowShouldClose(window))
        {
            glfwPollEvents();
            VulkanInstance::getInstance().BeginDrawFrame();
            VulkanInstance::getInstance().ResetCommandButter();
            ubo3.pos = glm::vec3(cos(t), 0.0f, 0.0f);
            UB.SetUniform(descriptorSet3, ubo3);
            Draw(VulkanInstance::getInstance().commandBuffers[VulkanInstance::getInstance().currentFrame]);
            VulkanInstance::getInstance().EndDrawFrame();
            VulkanInstance::getInstance().PresentFinalFrame();
            t += 0.0001f;
        }

        vkDeviceWaitIdle(VulkanInstance::getInstance().device);
    };

    void cleanup()
    {

        vbAllocator.Cleanup();
        UB.cleanUp(VulkanInstance::getInstance().device);
        VulkanInstance::getInstance().cleanup();

        glfwDestroyWindow(window);
        glfwTerminate();
    };

    void Draw(VkCommandBuffer commandBuffer)
    {
        VulkanInstance::getInstance().BeginCommandBuffer();
        VulkanInstance::getInstance().BeginRenderPass();
        VulkanInstance::getInstance().BindGraphicsPipeline(VulkanInstance::getInstance().graphicsPipeline);
        VulkanInstance::getInstance().SetViewportandScissor(commandBuffer);

        vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, VulkanInstance::getInstance().pipelineLayout, 0, 1, &descriptorSet3, 0, nullptr);

        vbAllocator.BindVertexBuffer(commandBuffer, vertexBuffer2);

        vkCmdDraw(commandBuffer, 3, 1, 0, 0);

        // vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, VulkanInstance::getInstance().pipelineLayout, 0, 1, &descriptorSet2, 0, nullptr);

        // vbAllocator.BindVertexBuffer(commandBuffer, vertexBuffer2);

        // vkCmdDraw(commandBuffer, 3, 1, 0, 0);

        // vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, VulkanInstance::getInstance().pipelineLayout, 0, 1, &descriptorSet3, 0, nullptr);

        // vbAllocator.BindVertexBuffer(commandBuffer, vertexBuffer2);

        // vkCmdDraw(commandBuffer, 3, 1, 0, 0);

        // vbAllocator.BindVertexBuffer(commandBuffer, vertexBuffer3);

        // vkCmdDraw(commandBuffer, 3, 1, 0, 0);

        VulkanInstance::getInstance().EndRenderPass();
        VulkanInstance::getInstance().EndCommandBuffer();
    };
};