#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define GLFW_INCLUDE_VULKAN

#include "GLFW/glfw3.h"
#include "vulkan/vulkan.h"

#include "type_def.h"

#ifndef VK_CORE_H
#define VK_CORE_H

VkApplicationInfo create_app_info();

VkInstance create_inst(VkApplicationInfo *app_info);

VCW_PhysicalDevice *get_phy_dev(VkInstance inst);

VCW_Device *create_dev(VCW_PhysicalDevice *vcw_phy_dev);

void update_surface_info(VCW_Surface *vcw_surf, VCW_PhysicalDevice vcw_phy_dev);

VCW_Surface *create_surf(VkInstance inst, VCW_PhysicalDevice vcw_phy_dev, VCW_Device vcw_dev, VkExtent2D dim);

VCW_Swapchain *create_swap(VCW_Device vcw_dev, VCW_Surface surf, VkSwapchainKHR old);

VCW_CommandPool create_cmd_pool(VCW_Device vcw_dev, VCW_Swapchain vcw_swap);

void clean_up_swap(VCW_Device vcw_dev, VCW_Swapchain swap);

void clean_up_cmd_pool(VCW_Device vcw_dev, VCW_CommandPool cmd_pool);
/*
void destroy_vk_core(VkInstance inst, VCW_Device *dev,
                     VCW_Swapchain *swap, VCW_Surface *surf,
                     VCW_RenderMgmt *rend_mgmt);
*/

#endif