#include <cglm/cglm.h>

#include "vulkan/vulkan.h"

#include "buffer.h"
#include "vk_core.h"

struct Vertex {
  vec2 pos;
  vec3 color;
};

struct UniformSet {
  mat4 model;
  mat4 view;
  mat4 proj;
};

struct RenderPassGroup {
  VkRenderPass rendp;
  uint32_t frame_buff_count;
  VkFramebuffer *frame_buffs;
};

struct PipeGroup {
  VkPipelineLayout pipe_layout;
  VkPipeline pipe;
};

#ifndef RENDER_H
#define RENDER_H

struct RenderPassGroup create_rendp(VkDevice dev, VkSurfaceFormatKHR form);
struct PipeGroup create_pipe(VkDevice dev, VkExtent2D extent,
                             VkRenderPass rendp);
void create_frame_buffs(struct RenderPassGroup *g_rendp, VkDevice dev,
                        uint32_t img_count, VkImageView *img_views,
                        VkExtent2D extent);
void render_frame(VkDevice dev, VkSwapchainKHR swap,
                  struct RenderMgmtGroup *rend_mgmt, VkQueue q_graph,
                  VkQueue q_pres);
void destroy_render(struct DevGroup *dev, struct PipeGroup *pipe,
                    struct RenderPassGroup *rendp);

#endif