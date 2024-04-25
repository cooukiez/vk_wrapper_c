#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

#include "render.h"

#define MAX_FRAMES_IN_FLIGHT 2

#define SCALE_RESOLUTION 0
#define RESOLUTION_SCALING_FACTOR 0.5f

int main(int argc, char **argv) {
    setvbuf(stdout, NULL, _IONBF, 0);
    fflush(stdout);

    // load_rdoc(); // used for renderdoc loading

    VkApplicationInfo app_info = create_app_info();
    VkInstance inst = create_inst(&app_info);
    // these are pointers, so you remember, that these objects are static
    VCW_PhysicalDevice *vcw_phy_dev = get_phy_dev(inst);
    VCW_Device *vcw_dev = create_dev(vcw_phy_dev);
    VCW_Surface *surf = create_surf(inst, *vcw_phy_dev, *vcw_dev, (VkExtent2D) {1920, 1080});
    VCW_Swapchain *swap = create_swap(*vcw_dev, *surf, VK_NULL_HANDLE);

    VCW_VkCoreGroup vcw_core = {vcw_phy_dev, vcw_dev, surf, swap};

    VCW_CommandPool cmd_pool = create_cmd_pool(*vcw_dev, swap->img_count);
    VCW_Renderpass rendp = create_rendp(*vcw_dev, *surf);
    if (SCALE_RESOLUTION)
        create_render_targets(*vcw_dev, *vcw_phy_dev, *surf, &rendp, swap->img_count,
                              (VkExtent2D) {swap->extent.width * RESOLUTION_SCALING_FACTOR,
                                            swap->extent.height * RESOLUTION_SCALING_FACTOR});
    create_frame_bufs(*vcw_dev, &rendp, swap->img_count, swap->img_views, swap->extent);
    VCW_Sync sync = create_sync(*vcw_dev, swap->img_count, MAX_FRAMES_IN_FLIGHT);
    //
    // create index buffer
    //
    uint32_t *indices = TRIANGLE_INDICES;
    uint32_t num_indices = NUM_TRIANGLE_INDICES;
    VCW_Buffer index_buf = create_index_buf(*vcw_dev, *vcw_phy_dev, indices, num_indices);
    //
    // create vertex buffer
    //
    Vertex *vertices = TRIANGLE_VERTICES;
    uint32_t num_vertices = NUM_TRIANGLE_VERTICES;
    VCW_Buffer vert_buf = create_vertex_buf(*vcw_dev, *vcw_phy_dev, vertices, num_vertices);
    //
    // create uniform buffers
    //
    uint32_t unif_buf_count = sync.max_frames;
    VCW_Buffer *unif_bufs = create_unif_bufs(*vcw_dev, *vcw_phy_dev, unif_buf_count);
    //
    // create descriptor pool
    //
    VCW_DescriptorPool vcw_desc = create_vcw_desc(unif_buf_count, 0);
    add_desc_set_layout(*vcw_dev, &vcw_desc, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL, 1);
    add_desc_set_layout(*vcw_dev, &vcw_desc, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL, 1);
    init_desc_pool(*vcw_dev, &vcw_desc);
    //
    // write uniform descriptor
    //
    for (uint32_t i = 0; i < unif_buf_count; i++) {
        write_buffer_desc(*vcw_dev, &vcw_desc, &unif_bufs[i], i, 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
    }

    VCW_Pipeline pipe = create_pipe(*vcw_dev, rendp, vcw_desc);

    VCW_Uniform unif;
    glm_mat4_identity(unif.data);

    VCW_PushConstant push_const;
    glm_mat4_zero(push_const.view);
    glm_vec2_zero(push_const.res);
    push_const.time = 0;

    VCW_App vcw_app;
    vcw_app.cmd = &cmd_pool;
    vcw_app.rendp = &rendp;
    vcw_app.pipe = &pipe;
    vcw_app.desc = &vcw_desc;
    vcw_app.sync = &sync;
    vcw_app.vert_buf = &vert_buf;
    vcw_app.num_vertices = num_vertices;
    vcw_app.index_buf = &index_buf;
    vcw_app.num_indices = num_indices;
    vcw_app.cpu_unif = &unif;
    vcw_app.cpu_push_const = &push_const;
    vcw_app.unif_bufs = unif_bufs;
    vcw_app.unif_buf_count = sync.max_frames;
    vcw_app.stats = malloc(sizeof(VCW_RenderStats));

    double center_x = swap->extent.width / 2.0;
    double center_y = swap->extent.height / 2.0;
    double last_x = center_x, last_y = center_y;

    VCW_Camera cam = create_default_cam(swap->extent);

    glfwSetInputMode(surf->window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    uint32_t frame_count = 0;
    double frame_time = 0;
    double cmd_record_time = 0;
    double img_acquire_time = 0;

    printf("\n");
    while (!glfwWindowShouldClose(surf->window)) {
        glfwPollEvents();
        //
        // submit
        //
        render(vcw_core, vcw_app);

        vcw_app.cpu_push_const->time += 1;
        /*

        double cursor_x, cursor_y;
        glfwGetCursorPos(surf->window, &cursor_x, &cursor_y);
        double dx = cursor_x - last_x;
        double dy = cursor_y - last_y;
        last_x = cursor_x;
        //
        last_y = cursor_y;

        update_cam_rotation(&cam, (float) dx, (float) dy);

        glm_vec2_copy((vec2) {(float) swap->extent.width, (float) swap->extent.height}, vcw_app.cpu_push_const->res);

        if (glfwGetKey(surf->window, GLFW_KEY_W) == GLFW_PRESS)
            glm_vec3_add(cam.pos, cam.mov_lin, cam.pos);
        if (glfwGetKey(surf->window, GLFW_KEY_S) == GLFW_PRESS)
            glm_vec3_sub(cam.pos, cam.mov_lin, cam.pos);
        if (glfwGetKey(surf->window, GLFW_KEY_A) == GLFW_PRESS)
            glm_vec3_add(cam.pos, cam.mov_lat, cam.pos);
        if (glfwGetKey(surf->window, GLFW_KEY_D) == GLFW_PRESS)
            glm_vec3_sub(cam.pos, cam.mov_lat, cam.pos);
        if (glfwGetKey(surf->window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
            cam.speed = CAM_FAST;
        else
            cam.speed = CAM_SLOW;

        write_view_proj_mat(cam, &push_const.view);
         */

        /*
        frame_time += vcw_app.stats->frame_time * 1000.0;
        frame_time /= 2;

        cmd_record_time += vcw_app.stats->cmd_record_time * 1000.0;
        cmd_record_time /= 2;

        img_acquire_time += vcw_app.stats->img_acquire_time * 1000.0;
        img_acquire_time /= 2;

        if (frame_count % 120 == 0) {
            char title[128];
            uint32_t fps = (uint32_t) (1.0 / (frame_time / 1000.0));
            sprintf(title, "Vk Wrapper - %fms | %d fps | %f ms | %f ms", frame_time, fps, cmd_record_time, img_acquire_time);
            glfwSetWindowTitle(surf->window, title);
        }
        frame_count++;
         */
    }

    vkDeviceWaitIdle(vcw_dev->dev);
    printf("command buffers finished.\n");

    destroy_render(*vcw_dev, pipe, rendp, sync);
    destroy_vk_core(inst, *vcw_dev, *swap, *surf, cmd_pool);

    return 0;
}