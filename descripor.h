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

#endif