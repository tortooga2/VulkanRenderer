#include "fileManagment.hpp"

#define USING_VALIDATION_LAYERS
#include "Graphics.hpp"

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
    // const int MAX_FRAMES_IN_FLIGHT = 2;
    // uint32_t currentFrame = 0;

    GLFWwindow *window;
    // VulkanInstance vkInst;

    void initWindow()
    {
        VKHelpers::CreateWindow(window, WIDTH, HEIGHT, "Nature Engine");
    };
    void initVulkan()
    {
        VulkanInstance::getInstance().initVulkan(window);
        VulkanInstance::getInstance().ChangeBackgroundColor(0.0f, 0.4f, 0.4f, 1.0f);
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
        }

        vkDeviceWaitIdle(VulkanInstance::getInstance().device);
    };

    void cleanup()
    {
        VulkanInstance::getInstance().cleanup();

        glfwDestroyWindow(window);
        glfwTerminate();
    };

    void Draw(VkCommandBuffer commandBuffer)
    {
        VulkanInstance::getInstance().BeginCommandBuffer();

        VulkanInstance::getInstance().BeginRenderPass();

        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, VulkanInstance::getInstance().graphicsPipeline);

        VulkanInstance::getInstance().SetViewportandScissor(commandBuffer);

        vkCmdDraw(commandBuffer, 3, 1, 0, 0);

        VulkanInstance::getInstance().EndRenderPass();
        VulkanInstance::getInstance().EndCommandBuffer();
    };
};