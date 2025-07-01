#include "fileManagment.hpp"

#include "Graphics.hpp"

#ifdef NDEBUG
const char *path = "./Shaders/";
#else
const char *path = "./Shaders/compiled/";
#endif

std::vector<VKHelpers::Vertex> triangles_vertices = {
    {{0.3f, -0.2f, 0.0f}, {1.0f, 0.0f, 0.0f}},
    {{0.8f, 0.8f, 0.0f}, {0.0f, 1.0f, 0.0f}},
    {{-0.2f, 0.8f, 0.0f}, {0.0f, 0.0f, 1.0f}}};

struct UniformBufferObject
{
    alignas(16) glm::vec3 color;
    alignas(16) glm::vec3 pos;
};

struct MeshObject {
    VkBuffer vertexBuffer;
    VkDescriptorSet descriptorSet;
    UniformBufferObject transform;


    void init(std::vector<VKHelpers::Vertex> &vertices, UBOMgr &UB, VkDescriptorSetLayout &descriptorSetLayout, VBAllocator<VKHelpers::Vertex> &vbAlloc){
        //TODO, make more general
        transform = {glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0)};
        UB.SubscribeToLayout(descriptorSetLayout, descriptorSet);
        vbAlloc.AddMesh(&vertices, vertexBuffer);


    }

    void draw(VulkanInstance *vkInst, VBAllocator<VKHelpers::Vertex> &vbAlloc, UBOMgr &UB){
        auto& commandBuffer = vkInst->commandBuffers[vkInst->currentFrame];
        UB.SetUniform(descriptorSet, transform);
        vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, vkInst->pipelineLayout, 0, 1, &descriptorSet, 0, nullptr);

        vbAlloc.BindVertexBuffer(commandBuffer, vertexBuffer);
        vkCmdDraw(commandBuffer, 3, 1, 0, 0);

    }

};

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



    UBOMgr UB;
    VkDescriptorSetLayout descriptorSetLayout;

    MeshObject triangle = {};


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
        
        //setup vertex and descriptor set data. 
        triangle.init(triangles_vertices, UB, descriptorSetLayout, vbAllocator);


        UB.CreatePool(vkInst->device);
        UB.AllocateDescriptors(vkInst->device, vkInst->physicalDevice);
        vbAllocator.AllocateVertexBuffer(vkInst->physicalDevice);
    };

    void mainLoop()
    {

        while (!glfwWindowShouldClose(window))
        {
            glfwPollEvents();
            vkInst->BeginDrawFrame();
            vkInst->ResetCommandButter();
            Update();
            
            Draw();
            vkInst->EndDrawFrame();
            vkInst->PresentFinalFrame();
            
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

    }

    void Draw()
    {
        VkCommandBuffer commandBuffer = vkInst->commandBuffers[vkInst->currentFrame];
        vkInst->BeginCommandBuffer();
        vkInst->BeginRenderPass();
        vkInst->SetViewportandScissor(commandBuffer);
        vkInst->BindGraphicsPipeline(vkInst->graphicsPipeline);
        

        triangle.draw(vkInst, vbAllocator, UB);

        vkInst->EndRenderPass();
        vkInst->EndCommandBuffer();
    };
};