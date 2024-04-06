#include "buffer.h"

uint32_t find_mem_type(uint32_t mem_type, VkMemoryPropertyFlags prop,
                       VkPhysicalDevice phy_dev) {
    VkPhysicalDeviceMemoryProperties mem_prop;
    vkGetPhysicalDeviceMemoryProperties(phy_dev, &mem_prop);

    for (uint32_t i = 0; i < mem_prop.memoryTypeCount; i++) {
        if ((mem_type & (1 << i)) &&
            (mem_prop.memoryTypes[i].propertyFlags & prop) == prop) {
            return i;
        }
    }

    return 0;
}

struct BufferSet create_buffer_info(size_t size, uint32_t alignment,
                                    VkBufferUsageFlagBits usage,
                                    VkSharingMode sharing) {
    return (struct BufferSet) {size, alignment, usage, sharing, 0, NULL, 0, 0, 0};
}

void init_buffer(VkDevice dev, struct BufferSet *buf) {
    buf->buf_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    buf->buf_info.size = buf->size;
    buf->buf_info.usage = buf->usage;
    buf->buf_info.sharingMode = buf->sharing;
    buf->buf_info.pNext = NULL;

    vkCreateBuffer(dev, &buf->buf_info, NULL, &buf->buf);
};

void allocate_memory(VkDevice dev, VkPhysicalDevice phy_dev,
                     struct BufferSet *buf) {
    vkGetBufferMemoryRequirements(dev, buf->buf, &buf->mem_req);

    buf->mem_type = find_mem_type(buf->mem_req.memoryTypeBits,
                                  VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                  VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                  phy_dev);

    printf("found memory type index: %x\n", buf->mem_type);

    VkMemoryAllocateInfo alloc_info;
    alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    alloc_info.allocationSize = buf->mem_req.size;
    alloc_info.memoryTypeIndex = buf->mem_type;
    alloc_info.pNext = NULL;

    vkAllocateMemory(dev, &alloc_info, NULL, &buf->mem);
    vkBindBufferMemory(dev, buf->buf, buf->mem, 0);
}

void init_mem(VkDevice dev, struct BufferSet *buf,
              void *data) {
    void *empty_data;
    vkMapMemory(dev, buf->mem, 0, buf->buf_info.size, 0, &empty_data);
    memcpy(empty_data, data, buf->size);
    vkUnmapMemory(dev, buf->mem);
}