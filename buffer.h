#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "vk_core.h"

#ifndef VCW_BUFFER_H
#define VCW_BUFFER_H

VCW_Buffer create_buffer(VCW_Device vcw_dev, size_t size, uint32_t alignment,
                         VkBufferUsageFlagBits usage, VkSharingMode sharing);

void allocate_memory(VCW_Device vcw_dev, VCW_PhysicalDevice phy_dev,
                     VCW_Buffer *buf);

void copy_data_to_buf(VCW_Device vcw_dev, VCW_Buffer *buf, void *data);

void map_mem(VCW_Device vcw_dev, VCW_Buffer *buf);

void unmap_mem(VCW_Device vcw_dev, VCW_Buffer *buf);

#endif //VCW_BUFFER_H