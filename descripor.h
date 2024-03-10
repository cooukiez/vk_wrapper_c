#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "vulkan/vulkan.h"

#include "buffer.h"

struct DescriptorGroup {
  VkDescriptorSetLayout *layouts;
  uint32_t set_count;

  VkDescriptorPool pool;
  VkDescriptorSet *sets;
};

#ifndef DESC_H
#define DESC_H

void add_desc_set_layout(struct DescriptorGroup *g_desc, VkDevice dev,
                         VkDescriptorType type, VkShaderStageFlags stage,
                         uint32_t binding_count);
void init_desc_pool(struct DescriptorGroup *g_desc, VkDevice dev,
                    VkDescriptorPoolSize *sizes, uint32_t size_count);
void write_buffer_desc(struct DescriptorGroup *g_desc, VkDevice dev,
                       struct BufferSet *g_buf, uint32_t set, uint32_t binding,
                       VkDescriptorType desc_type);

#endif