#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "vulkan/vulkan.h"

#include "type_def.h"
#include "buffer.h"

#ifndef DESC_H
#define DESC_H

VCW_DescriptorPool create_vcw_desc();
void add_desc_set_layout(VCW_Device vcw_dev, VCW_DescriptorPool *vcw_desc,
                         VkDescriptorType type, VkShaderStageFlags stage,
                         uint32_t binding_count);
void init_desc_pool(VCW_Device vcw_dev, VCW_DescriptorPool *vcw_desc,
                    VkDescriptorPoolSize *sizes, uint32_t size_count);
void write_buffer_desc(VCW_Device vcw_dev, VCW_DescriptorPool *vcw_desc,
                       VCW_Buffer *g_buf, uint32_t set, uint32_t binding,
                       VkDescriptorType desc_type);

#endif