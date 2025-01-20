# To-Do List

- [x] Figure out Structure for Vertex Buffer Creation
- [x] Create a Vertex Buffer Allocator
- [ ] Create Uniform Buffer Allocator
## Brain Storming - Vertex Buffer Allocator

- We want to allocation all memeory for our buffers with a single call. Inorder to create something like a Vertex Buffer, we need two objects. 1) VkBuffer and 2) VkDeviceMemory. Essentially we bind a buffer (with an offset) to a some mapped memory.
- We also must configure our graphics pipeline to with the correct vertex attribute pointers. Thus each buffer bound must have the same layout config.
- Right now there are two manditory pipelines we need. We need a pipeline to create the G-Buffers for albedo, position, normal, depth. Next we need to a Pipeline for actually combining results and output swapchain framebuffer.
  I need to think of some way to structure the memory allocation. For performance, it must happen in a single step. - Graphics Pipeline (Mandatory) For primative G-Buffers -> Not programmable - Graphics Pipeline (Mandatory) For final composite. -> Programmable. - Note output Framebuffer is set during the begin render pass phase to.

psuedo-code for VertexAllocator:
class VertexBufferAllocator:
std::vector<Vertex> meshes;
std::vector<uint32_t> offsets;
std::vector<VkBuffer \*> buffers //needed for a final call to Allocate()
VkDevice device
VkDeviceMemory memory

    Add(mesh, &vertexbuffer):
        - append current meshes size to offsets
        - append mesh to meshes
        - create VkBuffer info
        - create buffer at vertexbuffer
        - append pointer to vertexbuffer to buffers
        - clear mesh vector (although to be fair these would be created within the context of an init function to once we move on to mainloop, they would be destroyed anyway)

    Allocate(): //called at the end of Init() step
        - Finally actually allocate memory with VkAllocateMemory
        - itterate (i) through buffers and use vkBindBufferMemory, with offsets(i) as offset
        - finally map memory and copy memory and unmap memory (Fill Vertex Buffer)
        - clear meshes vector.




## Brain Storming - Uniform Buffer Allocator
- Immediatly what I am thinking is that out Uniform Buffer Object Struct, we just put what every the fuck we want into it. So like Model Matrix, proj, and view. Then we can also add time. Or like random position offset for instanced rendering or what have you. I can't image why I would want to have a uniform buffer array!? but it might be useful. right now thought I feel like it makes sense for this to be template. 


- So unlike a vertex buffer, where each object receives its own VkBuffer which was mapped to a specific region in the VkDeviceMemory we allocated. For Descriptor Sets, it works roughly the same. Accept for per object data we need, rather than a VkBuffer like in VB, we need a VkDescriptorSet. 

- For creating a Global Uniforms. Create a Struct and Allocate its own Buffer.
- For Individual Per-Object Data, it should be again and struct that you configure ahead of time, and then You Subscripe a Scene Object to it. 
- So we need a Create Global Descriptor Set function. A Create PerObject Description Set.  And Then finally a Subscribe to Descriptor Set. THen Finally a, Allocate Resources. Also an Update Descriptor Set.
- Challenge, the tutorial recomends having a Set for each frame in flight. 
- Vector<DescriptorSet> globalDescriptorSets; //for each frame in flight
- Vector<DescriptorSet> perObjectDescriptorSets; // For Each object we subscribe to it. We also then need to multiply this by the number of frames in flight.
- VkDescriptorPool descriptorPool; //for each frame in flight


There is a first descriptor pool which handles the a global uniform data. There is a set for each frame in flight. There is a second Descriptor pool. its max number equal to each Object * frames in flight. 

1. First Create Descriptor Layouts. (VkDescriptorSetLayouts)
2. Then We Create The PipelineLayout. (VkPipelineLayout)
3. Create a Descriptor Pool (vkCreateDescriptorPool)
4. Subscribe an Object to A Descriptor Layout (This should really just be a handle to an object (pointer maybe) stored in a vector) 
5. We allocate the required memory (This will be the Global Uniform Object size + N * per object buffer size FOR EACH LAYOUT DESCRIPTION WE ADD)
6. Finally we update our DescriptorSets to correct byte size of UBO and offset in buffer
7. To update memory using memcpy. 
    - One thing to note is that I still must store the offsets in the buffer for which to stride when copying data. 


##### Functions:
- CreateLayout<type>(layout)
- CreatPipelineLayout()

- SubscriptToSet(layout, handleID->VkDescriptorSet)
- CreateDescriptorPool()
- AllocateAndSetDescriptors() - Will iterate through 
- UpdateDescriptor(handle/ID, new UBO) -> will use a has map to find the location in memory then set it accordingly.



psuedo-Code for UniformBufferAllocator:
class UniformBufferAllocator:
    
    bufferSize = 0
    std::vector<DescriptorSetLayout> layouts;
    std::vector<Pools> pools
    std::map<VkDescriptorSet, offset)
    VkDescriptorSet Globals
    VkBuffer
    VkDeviceMemory
    void *data



    tamplate<typename T> CreateLayout<T>(layout):
        VkDescriptorSetLayoutBinding uboLayoutBinding{};
        uboLayoutBinding
    
    
