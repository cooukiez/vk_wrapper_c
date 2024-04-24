//
// Created by Ludw on 4/23/2024.
//

#include "vk_core.h"

#ifndef VCW_IMAGE_H
#define VCW_IMAGE_H

VCW_Image create_img(VCW_Device vcw_dev, VkExtent2D extent, VkFormat format, VkImageUsageFlags usage);

VCW_Image create_img_advanced(VCW_Device vcw_dev, VkImageCreateInfo img_info);

void create_img_memory(VCW_Device vcw_dev, VCW_PhysicalDevice vcw_phy_dev, VCW_Image *img);

void create_img_view(VCW_Device vcw_dev, VCW_Image *img);

void create_img_view_advanced(VCW_Device vcw_dev, VCW_Image *img, VkImageViewCreateInfo img_view_info);

void create_img_sampler(VCW_Device vcw_dev, VCW_Image *img, VkFilter filter, VkSamplerAddressMode address_mode);

void create_img_sampler_advanced(VCW_Device vcw_dev, VCW_Image *img, VkSamplerCreateInfo sampler_info);

void clean_up_img(VCW_Device vcw_dev, VCW_Image img);

#endif //VCW_IMAGE_H
