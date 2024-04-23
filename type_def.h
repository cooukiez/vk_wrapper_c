#include <cglm/cglm.h>

#include "GLFW/glfw3.h"
#include "vulkan/vulkan.h"

#ifndef VCW_TYPE_DEF_H
#define VCW_TYPE_DEF_H

//
// basis
//
typedef struct Vertex {
    vec3 pos;
    vec2 uv;
} Vertex;
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

    // does not have to be surface extent
    VkExtent2D window_extent;
    char resized;
    vec2 cursor_pos;
    vec2 cursor_delta;

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

typedef struct VCW_RenderStats {
    double frame_time;
    double last_swap_recreation_time;
} VCW_RenderStats;
//
// descriptor pool
//
typedef struct VCW_DescriptorPool {
    uint32_t size_count;
    VkDescriptorPoolSize *sizes;

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
    // only valid if map is called
    void *cpu_mem_pointer;
} VCW_Buffer;
//
// pipeline group
//
typedef struct VCW_PushConstant {
    mat4 view;
    vec2 res; // swapchain extent
    uint32_t time;
} VCW_PushConstant;

typedef struct VCW_Uniform {
    mat4 data;
} VCW_Uniform;

typedef struct VCW_App {
    VCW_CommandPool *cmd;
    VCW_Renderpass *rendp;
    VCW_Pipeline *pipe;
    VCW_DescriptorPool *desc;
    VCW_Sync *sync;

    VCW_Buffer *vert_buf;
    uint32_t num_vertices;
    VCW_Buffer *index_buf;
    uint32_t num_indices;

    VCW_Uniform *cpu_unif;
    VCW_PushConstant *cpu_push_const;

    VCW_Buffer *unif_bufs;
    uint32_t unif_buf_count;

    VCW_RenderStats *stats;
} VCW_App;

typedef struct VCW_Camera {
    mat4 proj;
    mat4 intermediate;

    vec3 pos;
    vec3 front;
    vec3 right;
    vec3 up;

    float yaw;
    float pitch;

    vec3 mov_lin; // forward / backward
    vec3 mov_lat; // sideways

    float speed;
    float sensitivity;
    float fov;

    float aspect_ratio; // height / width
    float near;
    float far;
} VCW_Camera;
//
// global definitions
//
extern VCW_PhysicalDevice *VCW_PHY_DEV;
extern VCW_Device *VCW_DEV;
extern VCW_Surface *VCW_SURF;
extern VCW_Swapchain *VCW_SWAP;

#endif //VCW_TYPE_DEF_H
