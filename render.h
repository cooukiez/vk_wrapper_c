#include "vulkan/vulkan.h"

#include "descripor.h"
#include "buffer.h"
#include "image.h"
#include "vert_data.h"
#include "camera.h"

#ifndef VCW_RENDER_H
#define VCW_RENDER_H

VCW_Renderpass create_rendp(VCW_Device vcw_dev, VCW_Surface surf);

VCW_Buffer
create_index_buf(VCW_Device vcw_dev, VCW_PhysicalDevice vcw_phy_dev, uint32_t *indices, uint32_t num_indices);

VCW_Buffer
create_vertex_buf(VCW_Device vcw_dev, VCW_PhysicalDevice vcw_phy_dev, Vertex *vertices, uint32_t num_vertices);

VCW_Buffer *create_unif_bufs(VCW_Device vcw_dev, VCW_PhysicalDevice vcw_phy_dev, uint32_t unif_buf_count);

VCW_Pipeline create_pipe(VCW_Device vcw_dev, VCW_Renderpass rendp, VCW_DescriptorPool vcw_desc);

void create_render_targets(VCW_Device vcw_dev, VCW_PhysicalDevice vcw_phy_dev, VCW_Surface vcw_surf,
                           VCW_Renderpass *vcw_rendp, uint32_t target_count, VkExtent2D extent);

void create_frame_bufs(VCW_Device vcw_dev, VCW_Renderpass *vcw_rendp, uint32_t frame_buf_count, VkImageView *attachs,
                       VkExtent2D extent);

VCW_Sync create_sync(VCW_Device vcw_dev, uint32_t img_count, uint32_t max_frames_in_flight);

void prepare_rendering(VCW_VkCoreGroup vcw_core, VCW_App vcw_app);

VCW_RenderResult render(VCW_VkCoreGroup vcw_core, VCW_App vcw_app);

void recreate_swap(VCW_VkCoreGroup vcw_core, VCW_App vcw_app);

void clean_up_frame_bufs(VCW_Device vcw_dev, VCW_Renderpass vcw_rendp);

void clean_up_sync(VCW_Device vcw_dev, VCW_Sync vcw_sync);

void destroy_render(VCW_Device vcw_dev, VCW_Pipeline vcw_pipe, VCW_Renderpass vcw_rendp, VCW_Sync vcw_sync);

#endif //VCW_RENDER_H