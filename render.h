#include "vulkan/vulkan.h"

#include "vk_core.h"
#include "util.h"
#include "descripor.h"
#include "buffer.h"


typedef struct Vertex {
    vec2 pos;
    vec3 color;
} Vertex;

#ifndef RENDER_H
#define RENDER_H

VCW_Renderpass create_rendp(VCW_Device vcw_dev, VCW_Surface surf);

VCW_Pipeline create_pipe(VCW_Device vcw_dev, VCW_Renderpass rendp, VCW_DescriptorPool vcw_desc, VkExtent2D extent);

void create_frame_bufs(VCW_Device vcw_dev, VCW_Swapchain vcw_swap, VCW_Renderpass *vcw_rendp, VkExtent2D extent);

VCW_Sync create_sync(VCW_Device vcw_dev, VCW_Swapchain vcw_swap, uint32_t max_frames_in_flight);

void prepare_rendering(VCW_VkCoreGroup vcw_core, VCW_App vcw_app);

VCW_RenderResult render(VCW_VkCoreGroup vcw_core, VCW_App vcw_app);

void recreate_swap(VCW_VkCoreGroup vcw_core, VCW_App vcw_app);

void clean_up_frame_bufs(VCW_Device vcw_dev, VCW_Renderpass vcw_rendp);

void clean_up_sync(VCW_Device vcw_dev, VCW_Sync vcw_sync);

void destroy_render(VCW_Device vcw_dev, VCW_Pipeline vcw_pipe, VCW_Renderpass vcw_rendp, VCW_Sync vcw_sync);

#endif