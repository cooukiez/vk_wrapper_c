#include <stdio.h>
#include <string.h>

#include "vulkan/vulkan.h"

struct DescriptorGroup {
  VkDescriptorSetLayout *layouts;
  uint32_t set_count;

  VkDescriptorPool pool;
  VkDescriptorSet *sets;
};

#ifndef BUFFER_H
#define BUFFER_H

#endif