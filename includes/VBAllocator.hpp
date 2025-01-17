#pragma once

#include "vulkanhelpers.hpp"
#include <map>

class VBAllocator
{
    std::vector<VKHelpers::Vertex> meshes;
    std::map<VkBuffer, VkDeviceSize> vertexBuffers;
    VkDeviceMemory vertexBufferMemory;
    VkDevice *device;

public:
    VBAllocator(VkDevice &device);
    ~VBAllocator();

    void AddMesh(std::vector<VKHelpers::Vertex> &mesh, VkBuffer &vertexBuffer);

    void AllocateVertexBuffer(VkPhysicalDevice &physicalDevice);

    void BindVertexBuffer(VkCommandBuffer &commandBuffer, VkBuffer &vertexBuffer);

    void Cleanup();

    uint32_t findMemoryType(uint32_t typeFilter, VkPhysicalDeviceMemoryProperties memProperties, VkMemoryPropertyFlags properties);
};
