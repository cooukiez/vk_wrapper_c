//
// Created by Ludw on 4/23/2024.
//

#include "image.h"

VCW_Image create_img(VCW_Device vcw_dev, VkExtent2D extent, VkFormat format, VkImageUsageFlags usage) {
    VCW_Image img;

    VkImageCreateInfo img_info;
    img_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    img_info.imageType = VK_IMAGE_TYPE_2D;
    img_info.extent.width = extent.width;
    img_info.extent.height = extent.height;
    img_info.extent.depth = 1;
    img_info.mipLevels = 1;
    img_info.arrayLayers = 1;
    img.format = format;
    img_info.format = img.format;
    img_info.tiling = VK_IMAGE_TILING_OPTIMAL;
    img_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    img_info.usage = usage;
    img_info.samples = VK_SAMPLE_COUNT_1_BIT;
    img_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    img_info.flags = 0;
    img_info.pNext = NULL;

    vkCreateImage(vcw_dev.dev, &img_info, NULL, &img.img);

    return img;
}

VCW_Image create_img_advanced(VCW_Device vcw_dev, VkImageCreateInfo img_info) {
    VCW_Image img;

    img.format = img_info.format;

    vkCreateImage(vcw_dev.dev, &img_info, NULL, &img.img);

    return img;
}

void create_img_memory(VCW_Device vcw_dev, VCW_PhysicalDevice vcw_phy_dev, VCW_Image *img) {
    vkGetImageMemoryRequirements(vcw_dev.dev, img->img, &img->mem_req);

    img->mem_type = find_mem_type(vcw_phy_dev, img->mem_req.memoryTypeBits,
                                  VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    VkMemoryAllocateInfo alloc_info;
    alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    alloc_info.allocationSize = img->mem_req.size;
    alloc_info.memoryTypeIndex = img->mem_type;
    alloc_info.pNext = NULL;

    vkAllocateMemory(vcw_dev.dev, &alloc_info, NULL, &img->mem);
    vkBindImageMemory(vcw_dev.dev, img->img, img->mem, 0);
}

void create_img_view(VCW_Device vcw_dev, VCW_Image *img) {
    VkImageViewCreateInfo img_view_info;
    img_view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    img_view_info.image = img->img;
    img_view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
    img_view_info.format = img->format;
    img_view_info.components = DEFAULT_COMPONENT_MAPPING;
    img_view_info.subresourceRange = DEFAULT_SUBRESOURCE_RANGE;
    img_view_info.flags = 0;
    img_view_info.pNext = NULL;

    vkCreateImageView(vcw_dev.dev, &img_view_info, NULL, &img->view);
}

void create_img_view_advanced(VCW_Device vcw_dev, VCW_Image *img, VkImageViewCreateInfo img_view_info) {
    vkCreateImageView(vcw_dev.dev, &img_view_info, NULL, &img->view);
}

void create_img_sampler(VCW_Device vcw_dev, VCW_Image *img, VkFilter filter, VkSamplerAddressMode address_mode) {
    VkSamplerCreateInfo sampler_info;
    sampler_info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    sampler_info.magFilter = filter;
    sampler_info.minFilter = filter;
    sampler_info.addressModeU = address_mode;
    sampler_info.addressModeV = address_mode;
    sampler_info.addressModeW = address_mode;
    sampler_info.anisotropyEnable = VK_FALSE;
    sampler_info.maxAnisotropy = 1;
    sampler_info.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    sampler_info.unnormalizedCoordinates = VK_FALSE;
    sampler_info.compareEnable = VK_FALSE;
    sampler_info.compareOp = VK_COMPARE_OP_ALWAYS;
    sampler_info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    sampler_info.mipLodBias = 0;
    sampler_info.minLod = 0;
    sampler_info.maxLod = 0;
    sampler_info.flags = 0;
    sampler_info.pNext = NULL;

    vkCreateSampler(vcw_dev.dev, &sampler_info, NULL, &img->sampler);
}

void create_img_sampler_advanced(VCW_Device vcw_dev, VCW_Image *img, VkSamplerCreateInfo sampler_info) {
    vkCreateSampler(vcw_dev.dev, &sampler_info, NULL, &img->sampler);
}

void clean_up_img(VCW_Device vcw_dev, VCW_Image img) {
    vkDestroySampler(vcw_dev.dev, img.sampler, NULL);
    vkDestroyImageView(vcw_dev.dev, img.view, NULL);
    vkFreeMemory(vcw_dev.dev, img.mem, NULL);
    vkDestroyImage(vcw_dev.dev, img.img, NULL);
}

