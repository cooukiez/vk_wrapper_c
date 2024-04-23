#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "render.h"

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

    VCW_CommandPool cmd_pool = create_cmd_pool(*vcw_dev, *swap);
    VCW_Renderpass rendp = create_rendp(*vcw_dev, *surf);
    create_frame_bufs(*vcw_dev, *swap, &rendp, swap->extent);
    VCW_Sync sync = create_sync(*vcw_dev, *swap, 2);
    //
    // create index buffer
    //
    uint32_t *indices = CUBE_INDICES;
    uint32_t num_indices = NUM_CUBE_INDICES;
    VCW_Buffer index_buf = create_index_buf(*vcw_dev, *vcw_phy_dev, indices, num_indices);
    //
    // create vertex buffer
    //
    Vertex *vertices = CUBE_VERTICES;
    uint32_t num_vertices = NUM_CUBE_VERTICES;
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

    VCW_Pipeline pipe = create_pipe(*vcw_dev, rendp, vcw_desc, swap->extent);

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

    printf("\n");
    while (!glfwWindowShouldClose(surf->window)) {
        glfwPollEvents();
        //
        // submit
        //
        render(vcw_core, vcw_app);
        vcw_app.cpu_push_const->time += 1;

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
    }

    vkDeviceWaitIdle(vcw_dev->dev);
    printf("command buffers finished.\n");

    destroy_render(*vcw_dev, pipe, rendp, sync);
    destroy_vk_core(inst, *vcw_dev, *swap, *surf, cmd_pool);

    return 0;
}