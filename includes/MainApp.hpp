#include "fileManagment.hpp"

#include "Graphics.hpp"

#ifdef NDEBUG
const char *path = "./Shaders/";
#else
const char *path = "./Shaders/compiled/";
#endif

std::vector<VKHelpers::Vertex> vertices = {
    {{-1.0f, -1.0f}, {1.0f, 0.0f, 0.0f}},
    {{1.0f, 1.0f}, {1.0f, 0.0f, 0.0f}},
    {{-0.5f, 0.5f}, {1.0f, 0.0f, 0.0f}}};

std::vector<VKHelpers::Vertex> vertices2 = {
    {{0.0f, -0.5f}, {0.0f, 1.0f, 0.0f}},
    {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
    {{-0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}}};

std::vector<VKHelpers::Vertex> vertices3 = {
    {{0.3f, -0.2f}, {1.0f, 0.0f, 0.0f}},
    {{0.8f, 0.8f}, {0.0f, 1.0f, 0.0f}},
    {{-0.2f, 0.8f}, {0.0f, 0.0f, 1.0f}}};

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
    VBAllocator vbAllocator;
    VkBuffer vertexBuffer;
    VkBuffer vertexBuffer2;
    VkBuffer vertexBuffer3;
    // VulkanInstance vkInst;

    void initWindow()
    {
        VKHelpers::CreateWindow(window, WIDTH, HEIGHT, "Nature Engine");
    };
    void initVulkan()
    {
        std::cout << "The Size of  Vertex is " << sizeof(VKHelpers::Vertex) << std::endl;
        VulkanInstance::getInstance().initVulkan(window);
        VulkanInstance::getInstance().ChangeBackgroundColor(0.0f, 0.0f, 0.0f, 1.0f);

        vbAllocator.AddMesh(&vertices, vertexBuffer);
        vbAllocator.AddMesh(&vertices2, vertexBuffer2);
        vbAllocator.AddMesh(&vertices3, vertexBuffer3);
        vbAllocator.AllocateVertexBuffer(VulkanInstance::getInstance().physicalDevice);
    };
    void mainLoop()
    {
        while (!glfwWindowShouldClose(window))
        {
            glfwPollEvents();
            VulkanInstance::getInstance().BeginDrawFrame();
            VulkanInstance::getInstance().ResetCommandButter();
            Draw(VulkanInstance::getInstance().commandBuffers[VulkanInstance::getInstance().currentFrame]);
            VulkanInstance::getInstance().EndDrawFrame();
            VulkanInstance::getInstance().PresentFinalFrame();
        }

        vkDeviceWaitIdle(VulkanInstance::getInstance().device);
    };

    void cleanup()
    {
        vbAllocator.Cleanup();
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

        vbAllocator.BindVertexBuffer(commandBuffer, vertexBuffer2);

        vkCmdDraw(commandBuffer, 3, 1, 0, 0);

        vbAllocator.BindVertexBuffer(commandBuffer, vertexBuffer);

        vkCmdDraw(commandBuffer, 3, 1, 0, 0);

        vbAllocator.BindVertexBuffer(commandBuffer, vertexBuffer3);

        vkCmdDraw(commandBuffer, 3, 1, 0, 0);

        VulkanInstance::getInstance().EndRenderPass();
        VulkanInstance::getInstance().EndCommandBuffer();
    };
};