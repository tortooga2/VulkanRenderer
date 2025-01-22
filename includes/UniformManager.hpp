#include "vulkanhelpers.hpp"

class UBOMgr
{
private:
    VkBuffer uniformBuffer;
    VkDeviceMemory uniformBufferMemory;

    VkDescriptorPool pool;

    int setSize;
    std::map<VkDescriptorSetLayout, int> LayoutAndStructSize;
    std::map<VkDescriptorSetLayout, std::vector<VkDescriptorSet *>> LayoutAndSet;
    std::map<VkDescriptorSet, VkDeviceSize> SetAndOffset;

    void *mappedData;

public:
    UBOMgr()
    {
        setSize = 0;
    }

    ~UBOMgr()
    {
    }

    void cleanUp(VkDevice &device)
    {
        for (auto &layout : LayoutAndSet)
        {
            vkDestroyDescriptorSetLayout(device, layout.first, nullptr);
        }
        vkDestroyBuffer(device, uniformBuffer, nullptr);
        vkFreeMemory(device, uniformBufferMemory, nullptr);
        vkDestroyDescriptorPool(device, pool, nullptr);
    }

    void CreateLayout(VkDevice &device, VkDescriptorSetLayout &layout, VkDeviceSize size)
    {
        VkDescriptorSetLayoutBinding uboLayoutBinding = {};
        uboLayoutBinding.binding = 0;
        uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        uboLayoutBinding.descriptorCount = 1;
        uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
        uboLayoutBinding.pImmutableSamplers = nullptr; // Optional

        VkDescriptorSetLayoutCreateInfo layoutInfo = {};
        layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layoutInfo.bindingCount = 1;
        layoutInfo.pBindings = &uboLayoutBinding;

        if (vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &layout) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create descriptor set layout!");
        }

        LayoutAndStructSize[layout] = size;

        std::cout << "Layout Created" << std::endl;
        std::cout << LayoutAndStructSize.size() << std::endl;
    }

    void SubscribeToLayout(VkDescriptorSetLayout &layout, VkDescriptorSet &set)
    {
        LayoutAndSet[layout].push_back(&set);
        std::cout << "Subscribed to Layout " << LayoutAndSet[layout].size() << std::endl;
        setSize++;
    }

    void CreatePool(VkDevice &device)
    {
        std::vector<VkDescriptorPoolSize> poolSizes;

        for (auto &layout : LayoutAndSet)
        {
            VkDescriptorPoolSize poolSize = {};
            poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            poolSize.descriptorCount = layout.second.size();
            poolSizes.push_back(poolSize);
        }

        VkDescriptorPoolCreateInfo poolInfo = {};
        poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
        poolInfo.pPoolSizes = poolSizes.data();
        poolInfo.maxSets = setSize;

        if (vkCreateDescriptorPool(device, &poolInfo, nullptr, &pool) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create descriptor pool!");
        }
    }

    void CreatePipelineLayout(VkDevice &device, VkPipelineLayout &pipelineLayout)
    {

        std::vector<VkDescriptorSetLayout> layouts;
        VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
        for (auto &layout : LayoutAndStructSize)
        {
            layouts.push_back(layout.first);
        }

        std::cout << "Layouts Size: " << layouts.size() << std::endl;

        if (layouts.size() != 0)
        {
            pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
            pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(layouts.size());
            pipelineLayoutInfo.pSetLayouts = layouts.data();
            pipelineLayoutInfo.pushConstantRangeCount = 0;    // Optional
            pipelineLayoutInfo.pPushConstantRanges = nullptr; // Optional
        }
        else
        {
            pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
            pipelineLayoutInfo.setLayoutCount = 0;
            pipelineLayoutInfo.pSetLayouts = nullptr;
            pipelineLayoutInfo.pushConstantRangeCount = 0;    // Optional
            pipelineLayoutInfo.pPushConstantRanges = nullptr; // Optional
        }

        if (vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create pipeline layout!");
        }
    }

    VkDeviceSize AlignMemory(VkDeviceSize alignment, VkDeviceSize offset)
    {
        return (offset + alignment - 1) & ~(alignment - 1);
    }

    uint32_t findMemoryType(uint32_t typeFilter, VkPhysicalDeviceMemoryProperties memProperties, VkMemoryPropertyFlags properties)
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

    void AllocateDescriptors(VkDevice &device, VkPhysicalDevice &physicalDevice)
    {
        VkPhysicalDeviceProperties deviceProperties;
        vkGetPhysicalDeviceProperties(physicalDevice, &deviceProperties);

        std::cout << "Currently Allocating Descriptors" << std::endl;
        VkDeviceSize bufferSize = 0;
        for (auto &layout : LayoutAndSet)
        {
            int StructSize = LayoutAndStructSize[layout.first];
            int SetCount = layout.second.size();

            std::cout << "About To Allocate " << SetCount << " Sets of " << StructSize << " Bytes" << std::endl;

            bufferSize += AlignMemory(deviceProperties.limits.minUniformBufferOffsetAlignment, StructSize) * SetCount;
        }

        for (auto &layout : LayoutAndSet)
        {
            std::vector<VkDescriptorSetLayout> layouts(layout.second.size(), layout.first);
            VkDescriptorSetAllocateInfo allocInfo{};
            allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
            allocInfo.descriptorPool = pool;
            allocInfo.descriptorSetCount = static_cast<uint32_t>(layouts.size());
            allocInfo.pSetLayouts = layouts.data();
            std::vector<VkDescriptorSet> descriptorSets(layout.second.size());
            if (vkAllocateDescriptorSets(device, &allocInfo, descriptorSets.data()) != VK_SUCCESS)
            {
                throw std::runtime_error("failed to allocate descriptor sets!");
            }

            for (int i = 0; i < layout.second.size(); i++)
            {
                *(layout.second[i]) = descriptorSets[i];
                std::cout << "Descriptor Set Allocated: " << descriptorSets[i] << std::endl;
            }
        }

        std::cout << "Total Buffer Size: " << bufferSize << std::endl;

        // Create Buffer
        VkBufferCreateInfo bufferInfo = {};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = bufferSize;
        bufferInfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        if (vkCreateBuffer(device, &bufferInfo, nullptr, &uniformBuffer) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create buffer");
        }

        VkMemoryRequirements memReqs;
        vkGetBufferMemoryRequirements(device, uniformBuffer, &memReqs);
        std::cout << "Memory Requirements: " << memReqs.alignment << std::endl;

        VkPhysicalDeviceMemoryProperties memProps;
        vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProps);

        VkMemoryAllocateInfo allocInfo = {};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memReqs.size;
        allocInfo.memoryTypeIndex = findMemoryType(memReqs.memoryTypeBits, memProps, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

        if (vkAllocateMemory(device, &allocInfo, nullptr, &uniformBufferMemory) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to allocate buffer memory");
        }

        std::cout << "Memory Allocated " << memReqs.size << " Bytes" << std::endl;

        vkBindBufferMemory(device, uniformBuffer, uniformBufferMemory, 0);

        vkMapMemory(device, uniformBufferMemory, 0, bufferSize, 0, &mappedData);

        // Update Descriptor Sets
        VkDeviceSize offset = 0;
        std::vector<VkDescriptorBufferInfo> bufferInfos;
        std::vector<VkWriteDescriptorSet> descriptorWrites;

        size_t totalDescriptorWrites = 0;
        for (auto &layout : LayoutAndSet)
        {
            totalDescriptorWrites += layout.second.size();
        }
        bufferInfos.reserve(totalDescriptorWrites);

        for (auto &layout : LayoutAndSet)
        {
            int StructSize = LayoutAndStructSize[layout.first];
            int SetCount = layout.second.size();

            for (int i = 0; i < SetCount; i++)
            {
                VkDescriptorBufferInfo bufferInfo = {};
                bufferInfo.buffer = uniformBuffer;
                bufferInfo.offset = offset;
                bufferInfo.range = StructSize;

                SetAndOffset[*layout.second[i]] = offset;

                std::cout << "Descriptor Set: " << *layout.second[i] << " Offset: " << offset << std::endl;

                bufferInfos.push_back(bufferInfo);
                offset += AlignMemory(memReqs.alignment, StructSize);

                VkWriteDescriptorSet descriptorWrite = {};
                descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                descriptorWrite.dstSet = *layout.second[i];
                descriptorWrite.dstBinding = 0;
                descriptorWrite.dstArrayElement = 0;

                descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                descriptorWrite.descriptorCount = 1;

                descriptorWrite.pBufferInfo = &bufferInfos.back(); // Optional
                descriptorWrite.pImageInfo = nullptr;              // Optional
                descriptorWrite.pTexelBufferView = nullptr;        // Optional

                descriptorWrites.push_back(descriptorWrite);
            }
        }

        std::cout << "Descriptor Writes Size: " << descriptorWrites.size() << std::endl;

        vkUpdateDescriptorSets(device, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
    }

    template <typename T>
    void SetUniform(VkDescriptorSet &descriptor, T ubo)
    {

        VkDeviceSize offset = SetAndOffset[descriptor];
        memcpy((char *)mappedData + offset, &ubo, sizeof(ubo));
    }
};