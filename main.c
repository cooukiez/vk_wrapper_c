#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "render.h"

int main(int argc, char **argv) {
    setvbuf(stdout, NULL, _IONBF, 0);
    fflush(stdout);

    //load_rdoc();

    VkApplicationInfo app_info = create_app_info();
    VkInstance inst = create_inst(&app_info);
    VCW_PhysicalDevice *phy_dev = get_phy_dev(inst);
    VCW_Device *dev = create_dev(phy_dev);
    VCW_Surface *surf = create_surf(inst, *phy_dev, *dev, (VkExtent2D) {1920, 1080});
    VCW_Swapchain *swap = create_swap(*dev, *surf, VK_NULL_HANDLE);

    VCW_VkCoreGroup vcw_core = {phy_dev, dev, surf, swap};

    VCW_CommandPool cmd_pool = create_cmd_pool(*dev, *swap);
    VCW_Renderpass rendp = create_rendp(*dev, *surf);
    create_frame_bufs(*dev, *swap, &rendp, swap->extent);

    //
    //
    // buffer creation part
    //
    // create index buffer
    //
    uint32_t indices[] = {0, 1, 2, 0, 3, 1};
    VCW_Buffer index_buf = create_buffer(*dev, sizeof(indices), sizeof(uint32_t),
                                         VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_SHARING_MODE_EXCLUSIVE);

    allocate_memory(*dev, *phy_dev, &index_buf);
    init_mem(*dev, &index_buf, &indices[0]);

    printf("index buffer created.\n");
    //
    // create vertex buffer
    //
    struct Vertex vertices[] = {{{0.0f,  -0.5f}, {1.0f, 0.0f, 0.0f}},
                                {{0.5f,  0.5f},  {0.0f, 1.0f, 0.0f}},
                                {{-0.5f, 0.5f},  {0.0f, 0.0f, 1.0f}}};

    VCW_Buffer vert_buf = create_buffer(*dev, sizeof(vertices), sizeof(struct Vertex),
                                        VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_SHARING_MODE_EXCLUSIVE);

    allocate_memory(*dev, *phy_dev, &vert_buf);
    init_mem(*dev, &vert_buf, &vertices[0]);

    printf("vertex buffer created.\n");
    //
    // create uniform buffer
    //
    struct UniformSet uniform[] = {{0, 0, 0}};
    VCW_Buffer uniform_buf = create_buffer(*dev, sizeof(uniform), sizeof(struct UniformSet),
                                           VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_SHARING_MODE_EXCLUSIVE);

    allocate_memory(*dev, *phy_dev, &uniform_buf);
    init_mem(*dev, &uniform_buf, &uniform[0]);

    printf("uniform buffer created.\n");
    //
    //
    // descriptor creation part
    //
    // create descriptor pool
    //
    VCW_DescriptorPool desc_group = create_vcw_desc();
    add_desc_set_layout(*dev, &desc_group, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                        VK_SHADER_STAGE_ALL, 1);

    VkDescriptorPoolSize unif_pool_size;
    unif_pool_size.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    unif_pool_size.descriptorCount =
            1; // number of individual descriptors of that type
    VkDescriptorPoolSize pool_sizes[] = {unif_pool_size};
    init_desc_pool(*dev, &desc_group, &pool_sizes[0], 1);
    //
    // write uniform descriptor
    //
    write_buffer_desc(*dev, &desc_group, &uniform_buf, 0, 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
    /*

    */

    VCW_Pipeline pipe = create_pipe(*dev, rendp, desc_group, swap->extent);
    VCW_Sync sync = create_sync(*dev, *swap, 2);

    VCW_PipelineGroup vcw_pipe_group = {&cmd_pool, &rendp, &pipe, &desc_group, &sync, &vert_buf, &index_buf};

    printf("\n");
    prepare_rendering(vcw_core, vcw_pipe_group);
    while (!glfwWindowShouldClose(surf->window)) {
        glfwPollEvents();
        if (surf->resized == 1) {
            surf->resized = 0;
            clock_t start = clock();
            for (uint32_t i = 0; i < sync.max_frames; i++) {
                vkWaitForFences(dev->dev, 1, &sync.fens[i], VK_TRUE, UINT64_MAX);
                vkResetFences(dev->dev, 1, &sync.fens[i]);
            }
            recreate_swap(vcw_core, vcw_pipe_group);
            clock_t end = clock();
            double elapsed = (double) (end - start) / CLOCKS_PER_SEC;
            printf("swapchain recreated in %f seconds.\n", elapsed);
        }
        //
        // submit
        //
        render(vcw_core, vcw_pipe_group);
    }
    vkDeviceWaitIdle(dev->dev);
    printf("command buffers finished.\n");

    destroy_render(*dev, pipe, rendp, sync);
    destroy_vk_core(inst, *dev, *swap,*surf, cmd_pool);

    return 0;
}