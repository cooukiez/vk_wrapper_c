#include "GLFW/glfw3.h"
#include "vulkan/vulkan.h"

#ifndef TYPE_DEF_H
#define TYPE_DEF_H

//
// vulkan core
//
typedef struct VCW_PhysicalDevice {
    VkPhysicalDevice dev;
    VkPhysicalDeviceProperties prop;
    VkPhysicalDeviceMemoryProperties mem_prop;
    VkDeviceSize mem_total;
} VCW_PhysicalDevice;

typedef struct VCW_Device {
    VkDevice dev;
    uint32_t qf_best_idx;
    VkQueue q_graph, q_pres;
    char single_queue;
} VCW_Device;

typedef struct VCW_Surface {
    GLFWwindow *window;

    // STORE NEW RESIZE
    VkExtent2D window_extent;
    char resized;

    VkSurfaceKHR surf;
    VkBool32 supported;
    VkSurfaceFormatKHR *forms;
    uint32_t form_count;
    char mailbox_mode_supported;

    // NEED UPDATING ON RESIZE
    VkSurfaceCapabilitiesKHR caps;
    char extent_suitable;
    VkExtent2D actual_extent;
} VCW_Surface;

typedef struct VCW_Swapchain {
    VkSwapchainKHR swap;
    VkExtent2D extent;
    uint32_t img_count;
    VkImage *imgs;
    VkImageView *img_views;
} VCW_Swapchain;

typedef struct VCW_CommandPool {
    VkCommandPool cmd_pool;
    // LENGTH OF SWAPCHAIN IMG_COUNT !!!
    uint32_t cmd_buf_count;
    VkCommandBuffer *cmd_bufs;
} VCW_CommandPool;

typedef struct VCW_VkCoreGroup {
    VCW_PhysicalDevice *phy_dev;
    VCW_Device *dev;
    VCW_Surface *surf;
    VCW_Swapchain *swap;
} VCW_VkCoreGroup;
//
// render
//
typedef struct VCW_Renderpass {
    VkRenderPass rendp;
    uint32_t frame_buf_count;
    VkFramebuffer *frame_bufs;
} VCW_Renderpass;

typedef struct VCW_Pipeline {
    VkPipelineLayout layout;
    VkPipeline pipe;
} VCW_Pipeline;

typedef struct VCW_Sync {
    VkFence *img_fens;
    uint32_t img_count;

    // THESE ARE MAX FRAMES IN FLIGHT !!!
    uint32_t max_frames;
    // LENGTH OF MAX FRAMES !!!
    VkSemaphore *img_avl_semps;
    VkSemaphore *rend_fin_semps;
    VkFence *fens;

    uint32_t cur_frame;
} VCW_Sync;

typedef enum VCW_RenderResult {
    VCW_SUCCESS = 0,
    VCW_ERROR = 1,
    VCW_OUT_OF_DATE = 2,
} VCW_RenderResult;
//
// descriptor pool
//
typedef struct VCW_DescriptorPool {
    VkDescriptorSetLayout *layouts;
    uint32_t set_count;

    VkDescriptorPool pool;
    VkDescriptorSet *sets;
} VCW_DescriptorPool;
//
// buffer
//
typedef struct VCW_Buffer {
    size_t size;
    uint32_t alignment;
    VkBufferUsageFlagBits usage;
    VkSharingMode sharing;

    VkBufferCreateInfo buf_info;
    VkBuffer buf;

    VkMemoryRequirements mem_req;
    uint32_t mem_type;
    VkDeviceMemory mem;
} VCW_Buffer;
//
// pipeline group
//
typedef struct VCW_PipelineGroup {
    VCW_CommandPool *cmd;
    VCW_Renderpass *rendp;
    VCW_Pipeline *pipe;
    VCW_DescriptorPool *desc;
    VCW_Sync *sync;
    VCW_Buffer *vert_buf;
    VCW_Buffer *index_buf;
} VCW_PipelineGroup;
//
// global definitions
//
extern VCW_PhysicalDevice *VCW_PHY_DEV;
extern VCW_Device *VCW_DEV;
extern VCW_Surface *VCW_SURF;
extern VCW_Swapchain *VCW_SWAP;

#endif //TYPE_DEF_H
