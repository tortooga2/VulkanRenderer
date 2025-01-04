#include <iostream>

#include <vulkan/vulkan.hpp>
#define VK_USE_PLATFORM_MACOS_MVK
#define GLFW_INCLUDE_VULKAN
#include <glfw/glfw3.h>
#define GLFW_EXPOSE_NATIVE_COCOA

#define SCR_WIDTH 800
#define SCR_HEIGHT 600


std::vector<const char*> getRequiredExtensions() {
        uint32_t glfwExtensionCount = 0;
        const char** glfwExtensions;
        glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
        std::vector<const char*> extensions;
    std::cout << "GLFW required extensions: " << glfwExtensionCount << std::endl;
        
        for(uint32_t i = 0; i < glfwExtensionCount; i++) {
            std::cout << glfwExtensions[i] << std::endl;
            extensions.push_back(glfwExtensions[i]);
        }

        extensions.push_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
        extensions.push_back("VK_KHR_get_physical_device_properties2");






        return extensions;
}

bool createInstance(){

        VkInstance _instance;

        VkApplicationInfo appInfo{};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = "Hello Triangle";
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName = "No Engine";
        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.apiVersion = VK_API_VERSION_1_2;

        VkInstanceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;

        createInfo.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;


        auto extensions = getRequiredExtensions();
        createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
        createInfo.ppEnabledExtensionNames = extensions.data();
        return vkCreateInstance(&createInfo, nullptr, &_instance) == VK_SUCCESS;




    }


int main()
{

    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    
    std::cout << "Hello, World!" << std::endl;
    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "OpenGLProject", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    if(createInstance()){
        std::cout << "Vulkan Available" << std::endl;
    }else{
        std::cout << "Vulkan Not Available" << std::endl;
    }
    glfwWindowHint(GLFW_DEPTH_BITS, 24); // Request a depth buffer with 24 bits

    while (!glfwWindowShouldClose(window))
    {

        glfwPollEvents();
    }

    glfwTerminate();

    return 0;
}
