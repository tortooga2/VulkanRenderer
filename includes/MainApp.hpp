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
    MainApp(int width, int height) : WIDTH(width), HEIGHT(height), vkInst(&VulkanInstance::getInstance()), vbAllocator(vkInst->device) {
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
    VulkanInstance *vkInst;

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

    struct UniformBufferObject
    {
        alignas(16) glm::vec3 color;
        alignas(16) glm::vec3 pos;
    };

    UniformBufferObject ubo1 = {glm::vec3(1.0f, 1.0f, 0.0f), glm::vec3(0.0)};
    UniformBufferObject ubo2 = {glm::vec3(1.0f, 0.0f, 1.0f), glm::vec3(0.0)};
    UniformBufferObject ubo3 = {glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.0)};

    float x = 0.0f;
    float y = 0.0f;

    void initWindow()
    {
        VKHelpers::CreateWindow(window, WIDTH, HEIGHT, "Nature Engine");
    };
    void initVulkan()
    {

        vkInst->initVulkan(window);
        UB.CreateLayout(vkInst->device, descriptorSetLayout, sizeof(UniformBufferObject));
        UB.CreatePipelineLayout(vkInst->device, vkInst->pipelineLayout);
        VKHelpers::CreateGraphicsPipeline(vkInst->device, vkInst->swapChainExtent, vkInst->pipelineLayout, vkInst->renderPass, vkInst->graphicsPipeline);
        UB.SubscribeToLayout(descriptorSetLayout, descriptorSet1);
        UB.SubscribeToLayout(descriptorSetLayout, descriptorSet2);
        UB.SubscribeToLayout(descriptorSetLayout, descriptorSet3);
        UB.CreatePool(vkInst->device);
        UB.AllocateDescriptors(vkInst->device, vkInst->physicalDevice);
        vkInst->ChangeBackgroundColor(0.0f, 0.0f, 0.0f, 1.0f);
        UB.SetUniform(descriptorSet1, ubo1);
        UB.SetUniform(descriptorSet2, ubo2);
        UB.SetUniform(descriptorSet3, ubo3);

        vbAllocator.AddMesh(&vertices, vertexBuffer);
        vbAllocator.AddMesh(&vertices2, vertexBuffer2);
        vbAllocator.AddMesh(&vertices3, vertexBuffer3);

        vbAllocator.AllocateVertexBuffer(vkInst->physicalDevice);
    };

    void mainLoop()
    {
        float t = 0.0f;
        while (!glfwWindowShouldClose(window))
        {
            glfwPollEvents();
            vkInst->BeginDrawFrame();
            vkInst->ResetCommandButter();
            Update();
            UB.SetUniform(descriptorSet3, ubo3);
            Draw(vkInst->commandBuffers[vkInst->currentFrame]);
            vkInst->EndDrawFrame();
            vkInst->PresentFinalFrame();
            t += 0.0001f;
        }

        vkDeviceWaitIdle(vkInst->device);
    };

    void cleanup()
    {

        vbAllocator.Cleanup();
        UB.cleanUp(vkInst->device);
        vkInst->cleanup();

        glfwDestroyWindow(window);
        glfwTerminate();
    };

    void Update()
    {
        if (glfwGetKey(window, GLFW_KEY_W))
        {
            y += 0.01f;
        }
        if (glfwGetKey(window, GLFW_KEY_S))
        {
            y -= 0.01f;
        }
        if (glfwGetKey(window, GLFW_KEY_A))
        {
            x -= 0.01f;
        }
        if (glfwGetKey(window, GLFW_KEY_D))
        {
            x += 0.01f;
        }
        ubo3.pos.x = x;
        ubo3.pos.y = -y;

        std::cout << "X: " << x << " Y: " << y << std::endl;
    }

    void Draw(VkCommandBuffer commandBuffer)
    {
        vkInst->BeginCommandBuffer();
        vkInst->BeginRenderPass();
        vkInst->BindGraphicsPipeline(vkInst->graphicsPipeline);
        vkInst->SetViewportandScissor(commandBuffer);

        vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, vkInst->pipelineLayout, 0, 1, &descriptorSet3, 0, nullptr);

        vbAllocator.BindVertexBuffer(commandBuffer, vertexBuffer3);
        vkCmdDraw(commandBuffer, 3, 1, 0, 0);

        vkInst->EndRenderPass();
        vkInst->EndCommandBuffer();
    };
};