#include <stdio.h>
#include <string.h>

#include "vulkan/vulkan.h"

#ifndef BUFFER_H
#define BUFFER_H

struct Vertex;

struct BufferSet {
  size_t size;
  uint32_t alignment;
  VkBufferUsageFlagBits usage;
  VkSharingMode sharing;

  VkBufferCreateInfo buf_info;
  VkBuffer buf;

  VkMemoryRequirements mem_req;
  uint32_t mem_type;
  VkDeviceMemory mem;
};

uint32_t find_mem_type(uint32_t mem_type, VkMemoryPropertyFlags prop,
                       VkPhysicalDevice phy_dev);

struct BufferSet create_buffer_info(size_t size, uint32_t alignment,
                                    VkBufferUsageFlagBits usage,
                                    VkSharingMode sharing);
void init_buffer(VkDevice dev, struct BufferSet *buf);

void allocate_memory(VkDevice dev, VkPhysicalDevice phy_dev,
                     struct BufferSet *buf);
void init_vert_mem(VkDevice dev, struct BufferSet *buf,
                   struct Vertex *vertices);

#endif