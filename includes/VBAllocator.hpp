#pragma once

#include "vulkanhelpers.hpp"
#include <map>

template <typename T>
class VBAllocator
{
    std::vector<std::vector<T> *> meshes;
    std::vector<VkDeviceSize> offsets;
    VkDeviceSize bufferSize = 0;

    std::map<VkBuffer *, VkDeviceSize> vertexBuffersMap;
    VkDeviceMemory vertexBufferMemory;
    VkDevice *device;

public:
    VBAllocator(VkDevice &device);
    ~VBAllocator();

    void AddMesh(std::vector<T> *mesh, VkBuffer &vertexBuffer);

    void AllocateVertexBuffer(VkPhysicalDevice &physicalDevice);

    void BindVertexBuffer(VkCommandBuffer &commandBuffer, VkBuffer &vertexBuffer);

    void Cleanup();

    VkDeviceSize AlignMemory(VkDeviceSize alignment, VkDeviceSize offset);

    uint32_t findMemoryType(uint32_t typeFilter, VkPhysicalDeviceMemoryProperties memProperties, VkMemoryPropertyFlags properties);
};

template <typename T>
VBAllocator<T>::VBAllocator(VkDevice &device) : device(&device)
{
    // CreateVertexBuffer();
}

template <typename T>
VBAllocator<T>::~VBAllocator()
{
    // Cleanup();
}

template <typename T>
void VBAllocator<T>::AddMesh(std::vector<T> *mesh, VkBuffer &vertexBuffer)
{
    meshes.push_back(mesh);
    offsets.push_back(bufferSize);
    std::cout << "Buffer size is " << sizeof((*mesh)[0]) * mesh->size() << std::endl;
    std::cout << "Offset is " << bufferSize << std::endl;
    bufferSize += sizeof((*mesh)[0]) * mesh->size();

    VkBufferCreateInfo bufferInfo = {};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = sizeof((*mesh)[0]) * mesh->size();
    bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(*device, &bufferInfo, nullptr, &vertexBuffer) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create vertex buffer!");
    }
    else
    {
        std::cout << "Vertex buffer created successfully" << std::endl;
    }
    vertexBuffersMap[&vertexBuffer] = offsets.back();
}

template <typename T>
void VBAllocator<T>::AllocateVertexBuffer(VkPhysicalDevice &physicalDevice)
{
    if (meshes.empty())
    {
        std::cout << "No meshes to allocate!" << std::endl;
        return;
    }

    VkMemoryRequirements memRequirements = {};
    memRequirements.size = 0;
    memRequirements.alignment = 0;
    memRequirements.memoryTypeBits = 0xFFFFFFFF;
    size_t i = 0;
    for (auto &buffer : vertexBuffersMap)
    {
        VkMemoryRequirements tempReqs;
        vkGetBufferMemoryRequirements(*device, *buffer.first, &tempReqs);
        memRequirements.size += tempReqs.size;
        memRequirements.alignment = std::max(memRequirements.alignment, tempReqs.alignment);
        memRequirements.memoryTypeBits |= tempReqs.memoryTypeBits;

        vertexBuffersMap[buffer.first] = AlignMemory(tempReqs.alignment, buffer.second);
        offsets[i] = vertexBuffersMap[buffer.first];
        ++i;
        std::cout << "Buffer offset is " << vertexBuffersMap[buffer.first] << std::endl;
    }

    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

    VkMemoryAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, memProperties, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    std::cout << "Memory size is " << memRequirements.size << std::endl;

    if (vkAllocateMemory(*device, &allocInfo, nullptr, &vertexBufferMemory) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to allocate vertex buffer memory!");
    }

    std::cout << "Vertex buffer memory allocated successfully" << std::endl;

    for (auto &buffer : vertexBuffersMap)
    {
        vkBindBufferMemory(*device, *buffer.first, vertexBufferMemory, buffer.second);
    }

    void *data;
    vkMapMemory(*device, vertexBufferMemory, 0, memRequirements.size, 0, &data);
    for (size_t i = 0; i < meshes.size(); i++)
    {
        memcpy((char *)data + offsets[i], meshes[i]->data(), sizeof((*meshes[i])[0]) * meshes[i]->size());
    }
    vkUnmapMemory(*device, vertexBufferMemory);
};

template <typename T>
void VBAllocator<T>::Cleanup()
{
    for (auto &buffer : vertexBuffersMap)
    {
        vkDestroyBuffer(*device, *buffer.first, nullptr);
    }
    vkFreeMemory(*device, vertexBufferMemory, nullptr);
}

template <typename T>
void VBAllocator<T>::BindVertexBuffer(VkCommandBuffer &commandBuffer, VkBuffer &vertexBuffer)
{
    VkBuffer vertexBuffersList[] = {vertexBuffer};
    VkDeviceSize offset = vertexBuffersMap[&vertexBuffer];
    VkDeviceSize offsets[] = {0};
    vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffersList, offsets);
}

template <typename T>
uint32_t VBAllocator<T>::findMemoryType(uint32_t typeFilter, VkPhysicalDeviceMemoryProperties memProperties, VkMemoryPropertyFlags properties)
{

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
    {
        if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
        {
            return i;
        }
    }

    throw std::runtime_error("failed to find suitable memory type!");
}

template <typename T>
VkDeviceSize VBAllocator<T>::AlignMemory(VkDeviceSize alignment, VkDeviceSize offset)
{
    return (offset + alignment - 1) & ~(alignment - 1);
}