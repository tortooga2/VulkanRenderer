# To-Do List

- [ ] Figure out Structure for Vertex Buffer Creation

## Brain Storming

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
