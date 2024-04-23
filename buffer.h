#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "vulkan/vulkan.h"

#include "type_def.h"

#ifndef VCW_BUFFER_H
#define VCW_BUFFER_H

uint32_t find_mem_type(VCW_PhysicalDevice vcw_phy_dev, uint32_t mem_type, VkMemoryPropertyFlags prop);

VCW_Buffer create_buffer(VCW_Device vcw_dev, size_t size, uint32_t alignment,
                         VkBufferUsageFlagBits usage, VkSharingMode sharing);

void allocate_memory(VCW_Device vcw_dev, VCW_PhysicalDevice phy_dev,
                     VCW_Buffer *buf);

void copy_data_to_buf(VCW_Device vcw_dev, VCW_Buffer *buf, void *data);

void map_mem(VCW_Device vcw_dev, VCW_Buffer *buf);

void unmap_mem(VCW_Device vcw_dev, VCW_Buffer *buf);

#endif //VCW_BUFFER_H