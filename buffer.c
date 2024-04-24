#include "buffer.h"

VCW_Buffer create_buffer(VCW_Device vcw_dev, size_t size, uint32_t alignment,
                         VkBufferUsageFlagBits usage, VkSharingMode sharing) {
    VCW_Buffer buf;

    buf.size = size;
    buf.alignment = alignment;
    buf.usage = usage;
    buf.sharing = sharing;
    buf.cpu_mem_pointer = NULL;

    buf.buf_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    buf.buf_info.size = buf.size;
    buf.buf_info.usage = buf.usage;
    buf.buf_info.sharingMode = buf.sharing;
    buf.buf_info.flags = 0;
    buf.buf_info.pNext = NULL;

    vkCreateBuffer(vcw_dev.dev, &buf.buf_info, NULL, &buf.buf);

    return buf;
}

void allocate_memory(VCW_Device vcw_dev, VCW_PhysicalDevice vcw_phy_dev, VCW_Buffer *buf) {
    vkGetBufferMemoryRequirements(vcw_dev.dev, buf->buf, &buf->mem_req);

    buf->mem_type = find_mem_type(vcw_phy_dev, buf->mem_req.memoryTypeBits,
                                  VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    printf("found memory type index: %x\n", buf->mem_type);

    VkMemoryAllocateInfo alloc_info;
    alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    alloc_info.allocationSize = buf->mem_req.size;
    alloc_info.memoryTypeIndex = buf->mem_type;
    alloc_info.pNext = NULL;

    vkAllocateMemory(vcw_dev.dev, &alloc_info, NULL, &buf->mem);
    vkBindBufferMemory(vcw_dev.dev, buf->buf, buf->mem, 0);
}

void copy_data_to_buf(VCW_Device vcw_dev, VCW_Buffer *buf, void *data) {
    void *cpu_mem_pointer;
    vkMapMemory(vcw_dev.dev, buf->mem, 0, buf->buf_info.size, 0, &cpu_mem_pointer);
    memcpy(cpu_mem_pointer, data, buf->size);
    vkUnmapMemory(vcw_dev.dev, buf->mem);
}

void map_mem(VCW_Device vcw_dev, VCW_Buffer *buf) {
    vkMapMemory(vcw_dev.dev, buf->mem, 0, buf->buf_info.size, 0, &buf->cpu_mem_pointer);
}

void unmap_mem(VCW_Device vcw_dev, VCW_Buffer *buf) {
    vkUnmapMemory(vcw_dev.dev, buf->mem);
}