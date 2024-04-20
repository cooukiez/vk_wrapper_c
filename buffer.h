#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "vulkan/vulkan.h"

#include "type_def.h"

#ifndef BUFFER_H
#define BUFFER_H

struct Vertex;

uint32_t find_mem_type(VCW_PhysicalDevice vcw_phy_dev, uint32_t mem_type, VkMemoryPropertyFlags prop);

VCW_Buffer create_buffer(VCW_Device vcw_dev, size_t size, uint32_t alignment,
                         VkBufferUsageFlagBits usage, VkSharingMode sharing);

void allocate_memory(VCW_Device vcw_dev, VCW_PhysicalDevice phy_dev,
                     VCW_Buffer *buf);

void init_mem(VCW_Device vcw_dev, VCW_Buffer *buf, void *data);

#endif