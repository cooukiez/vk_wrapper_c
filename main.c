#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "render.h"

int main(int argc, char **argv) {
  setvbuf(stdout, NULL, _IONBF, 0);

  VkApplicationInfo app_info = create_app_info();
  VkInstance inst = create_inst(&app_info);
  struct PhyDevGroup phy_dev = get_phy_dev(inst);
  struct DevGroup dev = create_dev(phy_dev.dev);
  struct SurfaceGroup surf =
      create_surf(inst, phy_dev.dev, dev.qf_best_idx, (VkExtent2D){1920, 1080});
  struct SwapchainGroup swap =
      create_swap(dev.dev, VK_NULL_HANDLE, surf.surf, surf.caps, surf.forms[0],
                  surf.extent_suitable, surf.actual_extent, dev.single_queue,
                  surf.mailbox_mode_supported);

  struct RenderPassGroup rendp = create_rendp(dev.dev, surf.forms[0]);
  create_frame_buffs(&rendp, dev.dev, swap.img_count, swap.img_views,
                     swap.extent);
  struct PipeGroup pipe = create_pipe(dev.dev, swap.extent, rendp.rendp);
  struct RenderMgmtGroup rend_mgmt =
      create_rend_mgmt(dev.dev, dev.qf_best_idx, swap.img_count, swap.extent,
                       rendp.rendp, rendp.frame_buffs, pipe.pipe);

  //
  //
  // buffer creation part
  //
  // create index buffer
  //
  uint32_t indices[] = {0, 1, 2, 0, 3, 1};
  struct BufferSet index_buf = create_buffer_info(
      sizeof(indices), sizeof(uint32_t), VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
      VK_SHARING_MODE_EXCLUSIVE);

  init_buffer(dev.dev, &index_buf);
  allocate_memory(dev.dev, phy_dev.dev, &index_buf);
  init_mem(dev.dev, &index_buf, &indices[0]);

  printf("index buffer created.\n");
  //
  // create vertex buffer
  //
  struct Vertex vertices[] = {{{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
                              {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
                              {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}};

  struct BufferSet vert_buf = create_buffer_info(
      sizeof(vertices), sizeof(struct Vertex),
      VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_SHARING_MODE_EXCLUSIVE);

  init_buffer(dev.dev, &vert_buf);
  allocate_memory(dev.dev, phy_dev.dev, &vert_buf);
  init_mem(dev.dev, &vert_buf, &vertices[0]);

  printf("vertex buffer created.\n");
  //
  // create uniform buffer
  //
  struct UniformSet uniform[] = {{0, 0, 0}};
  struct BufferSet uniform_buf = create_buffer_info(
      sizeof(uniform), sizeof(struct UniformSet),
      VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_SHARING_MODE_EXCLUSIVE);

  init_buffer(dev.dev, &uniform_buf);
  allocate_memory(dev.dev, phy_dev.dev, &uniform_buf);
  init_mem(dev.dev, &uniform_buf, &uniform[0]);
  
  printf("uniform buffer created.\n");
  /*
  //
  //create vertex input binding description
  //
  VkVertexInputBindingDescription
          vert_bindg_desc;
  vert_bindg_desc.binding=0;
  vert_bindg_desc.stride=sizeof(struct Vertex);
  vert_bindg_desc.inputRate=
          VK_VERTEX_INPUT_RATE_VERTEX;
  printf("vertex input binding description created.\n");
  //
  //create vertex attribute description
  //
  VkVertexInputAttributeDescription
          vert_attr_desc;
  vert_attr_desc.binding=0;
  vert_attr_desc.location=0;
  vert_attr_desc.format=
          VK_FORMAT_R32G32_SFLOAT;
  vert_attr_desc.offset=
          offsetof(struct Vertex, pos);
  printf("vertex attribute description created.\n");
  //
  //create uniform buffer
  //
  VkBuffer uni_buf;
  VkDeviceMemory uni_buf_mem;
  VkDeviceSize uni_buf_size=
          sizeof(struct UniformBufferObject);
  printf("uniform buffer created.\n");
  //
  //create uniform descriptor layout binding
  //
  VkDescriptorSetLayoutBinding ubo_bindg_lay;
  ubo_bindg_lay.binding=0;
  ubo_bindg_lay.descriptorType=
          VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  ubo_bindg_lay.descriptorCount=1;
  ubo_bindg_lay.stageFlags=
          VK_SHADER_STAGE_VERTEX_BIT;
  ubo_bindg_lay.pImmutableSamplers=NULL;
  printf("uniform buffer binding layout created.\n");
  //
  //create uniform descriptor set layout
  //
  VkDescriptorSetLayout ubo_lay;
  VkDescriptorSetLayoutCreateInfo ubo_lay_cre_info;
  ubo_lay_cre_info.sType=
          VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
  ubo_lay_cre_info.bindingCount=1;
  ubo_lay_cre_info.pBindings=&ubo_bindg_lay;
  vkCreateDescriptorSetLayout(dev,&ubo_lay_cre_info,NULL,&ubo_lay);
  printf("uniform buffer set layout created.\n");
  */

  printf("\n");
  while (!glfwWindowShouldClose(surf.window)) {
    glfwPollEvents();
    //
    // submit
    //
    render_frame(dev.dev, swap.swap, &rend_mgmt, dev.q_graph, dev.q_pres);
  }
  vkDeviceWaitIdle(dev.dev);
  printf("command buffers finished.\n");
  destroy_render(&dev, &pipe, &rendp);
  destroy_vk_core(inst, &dev, &swap, &surf, &rend_mgmt);

  return 0;
}