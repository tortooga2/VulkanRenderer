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
        uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
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
    }

    void SubscribeToLayout(VkDescriptorSetLayout &layout, VkDescriptorSet &set)
    {
        LayoutAndSet[layout].push_back(&set);
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
        for (auto &layout : LayoutAndSet)
        {
            layouts.push_back(layout.first);
        }
        VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(layouts.size());
        pipelineLayoutInfo.pSetLayouts = layouts.data();
        pipelineLayoutInfo.pushConstantRangeCount = 0;    // Optional
        pipelineLayoutInfo.pPushConstantRanges = nullptr; // Optional

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
        VkDeviceSize bufferSize = 0;
        for (auto &layout : LayoutAndSet)
        {
            int StructSize = LayoutAndStructSize[layout.first];
            int SetCount = layout.second.size();

            bufferSize += StructSize * SetCount;
        }
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

        vkBindBufferMemory(device, uniformBuffer, uniformBufferMemory, 0);

        // Update Descriptor Sets
        VkDeviceSize offset = 0;
        std::vector<VkDescriptorBufferInfo> bufferInfos;
        std::vector<VkWriteDescriptorSet> descriptorWrites;
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

                bufferInfos.push_back(bufferInfo);
                offset += AlignMemory(memReqs.alignment, StructSize);

                VkWriteDescriptorSet descriptorWrite = {};
                descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                descriptorWrite.dstSet = *layout.second[i];
                descriptorWrite.dstBinding = 0;
                descriptorWrite.dstArrayElement = 0;

                descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                descriptorWrite.descriptorCount = 1;

                descriptorWrite.pBufferInfo = &bufferInfos.back();
                descriptorWrite.pImageInfo = nullptr;       // Optional
                descriptorWrite.pTexelBufferView = nullptr; // Optional
            }
        }

        vkUpdateDescriptorSets(device, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
    }
};