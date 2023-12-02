#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define GLFW_INCLUDE_VULKAN

#include "GLFW/glfw3.h"
#include "vulkan/vulkan.h"

struct PhyDevGroup {
  VkPhysicalDevice dev;
  VkPhysicalDeviceProperties prop;
  VkPhysicalDeviceMemoryProperties mem_prop;
  VkDeviceSize mem_total;
};

struct DevGroup {
  VkDevice dev;
  uint32_t qf_best_idx;
  VkQueue q_graph, q_pres;
  char single_queue;
};

struct SurfaceGroup {
  GLFWwindow *window;
  VkSurfaceKHR surf;
  VkBool32 supported;
  VkSurfaceFormatKHR *forms;
  VkSurfaceFormatKHR form_count;
  char mailbox_mode_supported;

  // NEED UPDATING ON RESIZE
  VkSurfaceCapabilitiesKHR caps;
  char extent_suitable;
  VkExtent2D actual_extent;
};

struct SwapchainGroup {
  VkSwapchainKHR swap;
  VkExtent2D extent;
  uint32_t img_count;
  VkImage *imgs;
  VkImageView *img_views;
};

struct RenderMgmtGroup {
  VkCommandPool cmd_pool;
  // LENGTH OF IMG_COUNT !!!
  VkCommandBuffer *cmd_buffs;
  VkFence *img_fens;
  uint32_t img_count;

  // THESE ARE MAX FRAMES IN FLIGHT !!!
  uint32_t max_frames;
  // LENGTH OF MAX FRAMES !!!
  VkSemaphore *img_avl_semps;
  VkSemaphore *rend_fin_semps;
  VkFence *fens;

  uint32_t cur_frame;
};

#ifndef VK_CORE_H
#define VK_CORE_H

VkApplicationInfo create_app_info();
VkInstance create_inst(VkApplicationInfo *app_info);
struct PhyDevGroup get_phy_dev(VkInstance inst);
struct DevGroup create_dev(VkPhysicalDevice phy_dev);
struct SurfaceGroup create_surf(VkInstance inst, VkPhysicalDevice phy_dev,
                                size_t qf_best_idx, VkExtent2D dim);
struct SwapchainGroup create_swap(VkDevice dev, VkSwapchainKHR old,
                                  VkSurfaceKHR surf,
                                  VkSurfaceCapabilitiesKHR caps,
                                  VkSurfaceFormatKHR form, char extent_suitable,
                                  VkExtent2D actual_extent, char single_queue,
                                  char mailbox_mode_supported);
struct RenderMgmtGroup create_rend_mgmt(VkDevice dev, uint32_t qf_best_idx,
                                        uint32_t img_count, VkExtent2D extent,
                                        VkRenderPass rendp,
                                        VkFramebuffer *frame_buffs,
                                        VkPipeline pipe);
void destroy_vk_core(VkInstance inst, struct DevGroup *dev,
                     struct SwapchainGroup *swap, struct SurfaceGroup *surf,
                     struct RenderMgmtGroup *rend_mgmt);

#endif