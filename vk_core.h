#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define GLFW_INCLUDE_VULKAN

#include "type_def.h"

#ifndef VCW_VK_CORE_H
#define VCW_VK_CORE_H

extern VkComponentMapping DEFAULT_COMPONENT_MAPPING;
extern VkImageSubresourceRange DEFAULT_SUBRESOURCE_RANGE;

VkApplicationInfo create_app_info();

VkInstance create_inst(VkApplicationInfo *app_info);

VCW_PhysicalDevice *get_phy_dev(VkInstance inst);

VCW_Device *create_dev(VCW_PhysicalDevice *vcw_phy_dev);

void update_surface_info(VCW_Surface *vcw_surf, VCW_PhysicalDevice vcw_phy_dev);

VCW_Surface *create_surf(VkInstance inst, VCW_PhysicalDevice vcw_phy_dev, VCW_Device vcw_dev, VkExtent2D dim);

VCW_Swapchain *create_swap(VCW_Device vcw_dev, VCW_Surface surf, VkSwapchainKHR old);

VCW_CommandPool create_cmd_pool(VCW_Device vcw_dev, uint32_t cmd_buf_count);

uint32_t find_mem_type(VCW_PhysicalDevice vcw_phy_dev, uint32_t mem_type, VkMemoryPropertyFlags prop);

void clean_up_swap(VCW_Device vcw_dev, VCW_Swapchain swap);

void clean_up_cmd_pool(VCW_Device vcw_dev, VCW_CommandPool cmd_pool);

void destroy_vk_core(VkInstance inst, VCW_Device vcw_dev, VCW_Swapchain vcw_swap, VCW_Surface vcw_surf,
                     VCW_CommandPool vcw_cmd);

#endif //VCW_VK_CORE_H