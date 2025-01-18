#pragma once

#include "vulkanhelpers.hpp"
#include <map>

class VBAllocator
{
    std::vector<std::vector<VKHelpers::Vertex> *> meshes;
    std::vector<VkDeviceSize> offsets;
    VkDeviceSize bufferSize = 0;

    std::map<VkBuffer *, VkDeviceSize> vertexBuffersMap;
    VkDeviceMemory vertexBufferMemory;
    VkDevice *device;

public:
    VBAllocator(VkDevice &device);
    ~VBAllocator();

    void AddMesh(std::vector<VKHelpers::Vertex> *mesh, VkBuffer &vertexBuffer);

    void AllocateVertexBuffer(VkPhysicalDevice &physicalDevice);

    void BindVertexBuffer(VkCommandBuffer &commandBuffer, VkBuffer &vertexBuffer);

    void Cleanup();

    VkDeviceSize AlignMemory(VkDeviceSize alignment, VkDeviceSize offset);

    uint32_t findMemoryType(uint32_t typeFilter, VkPhysicalDeviceMemoryProperties memProperties, VkMemoryPropertyFlags properties);
};
