#include "../includes/VBAllocator.hpp"

VBAllocator::VBAllocator(VkDevice &device) : device(&device)
{
    // CreateVertexBuffer();
}

VBAllocator::~VBAllocator()
{
    // Cleanup();
}

void VBAllocator::AddMesh(std::vector<VKHelpers::Vertex> &mesh, VkBuffer &vertexBuffer)
{
    VkDeviceSize offset = meshes.size() * sizeof(meshes[0]);
    meshes.insert(meshes.end(), mesh.begin(), mesh.end());

    VkBufferCreateInfo bufferInfo = {};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = sizeof(mesh[0]) * mesh.size();
    bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    if (vkCreateBuffer(*device, &bufferInfo, nullptr, &vertexBuffer) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create vertex buffer!");
    }
    vertexBuffers[vertexBuffer] = offset;
}

void VBAllocator::AllocateVertexBuffer(VkPhysicalDevice &physicalDevice)
{
    if (meshes.empty())
    {
        std::cout << "No meshes to allocate!" << std::endl;
        return;
    }

    VkDeviceSize bufferSize = sizeof(meshes[0]) * meshes.size();

    VkMemoryRequirements memRequirements = {};
    memRequirements.alignment = 1;
    memRequirements.memoryTypeBits = 0xFFFFFFFF;
    for (auto &buffer : vertexBuffers)
    {
        VkMemoryRequirements tempMemRequirements;
        vkGetBufferMemoryRequirements(*device, buffer.first, &tempMemRequirements);
        memRequirements.size += tempMemRequirements.size;
        memRequirements.alignment = std::max(memRequirements.alignment, tempMemRequirements.alignment);
        memRequirements.memoryTypeBits &= tempMemRequirements.memoryTypeBits;
    }

    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

    VkMemoryAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, memProperties, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    if (vkAllocateMemory(*device, &allocInfo, nullptr, &vertexBufferMemory) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to allocate vertex buffer memory!");
    }
    else
    {
        std::cout << "Vertex buffer memory allocated successfully " << memRequirements.size << std::endl;
    }

    for (auto buffer : vertexBuffers)
    {
        std::cout << "Binding buffer to memory " << buffer.second << std::endl;
        vkBindBufferMemory(*device, buffer.first, vertexBufferMemory, buffer.second);
    }

    void *data;
    vkMapMemory(*device, vertexBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, meshes.data(), (size_t)bufferSize);
    vkUnmapMemory(*device, vertexBufferMemory);

    meshes.clear();
    meshes.shrink_to_fit();
};

void VBAllocator::Cleanup()
{
    for (auto &buffer : vertexBuffers)
    {
        vkDestroyBuffer(*device, buffer.first, nullptr);
    }
    vkFreeMemory(*device, vertexBufferMemory, nullptr);
}

void VBAllocator::BindVertexBuffer(VkCommandBuffer &commandBuffer, VkBuffer &vertexBuffer)
{
    VkBuffer vertexBuffersList[] = {vertexBuffer};
    VkDeviceSize offset = vertexBuffers[vertexBuffer];
    VkDeviceSize offsets[] = {offset};
    vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffersList, offsets);
}

uint32_t VBAllocator::findMemoryType(uint32_t typeFilter, VkPhysicalDeviceMemoryProperties memProperties, VkMemoryPropertyFlags properties)
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