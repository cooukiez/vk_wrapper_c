#include "render.h"

struct RenderPassGroup create_rendp(VkDevice dev, VkSurfaceFormatKHR form) {
  //
  // fill attachment description
  //
  VkAttachmentDescription attach_desc;
  attach_desc.flags = 0;
  attach_desc.format = form.format;
  attach_desc.samples = VK_SAMPLE_COUNT_1_BIT;
  attach_desc.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  attach_desc.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
  attach_desc.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  attach_desc.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  attach_desc.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  attach_desc.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
  printf("attachment description filled.\n");
  //
  // fill attachment reference
  //
  VkAttachmentReference attach_ref;
  attach_ref.attachment = 0;
  attach_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
  printf("attachment reference filled.\n");
  //
  // fill subpass description
  //
  VkSubpassDescription subp_desc;
  subp_desc.flags = 0;
  subp_desc.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
  subp_desc.inputAttachmentCount = 0;
  subp_desc.pInputAttachments = NULL;
  subp_desc.colorAttachmentCount = 1;
  subp_desc.pColorAttachments = &attach_ref;
  subp_desc.pResolveAttachments = NULL;
  subp_desc.pDepthStencilAttachment = NULL;
  subp_desc.preserveAttachmentCount = 0;
  subp_desc.pPreserveAttachments = NULL;
  printf("subpass description filled.\n");
  //
  // fill subpass dependency
  //
  VkSubpassDependency subp_dep;
  subp_dep.srcSubpass = VK_SUBPASS_EXTERNAL;
  subp_dep.dstSubpass = 0;
  subp_dep.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  subp_dep.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  subp_dep.srcAccessMask = 0;
  subp_dep.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
  subp_dep.dependencyFlags = 0;
  printf("subpass dependency created.\n");
  //
  // create render pass
  //
  VkRenderPassCreateInfo rendp_info;
  rendp_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
  rendp_info.pNext = NULL;
  rendp_info.flags = 0;
  rendp_info.attachmentCount = 1;
  rendp_info.pAttachments = &attach_desc;
  rendp_info.subpassCount = 1;
  rendp_info.pSubpasses = &subp_desc;
  rendp_info.dependencyCount = 1;
  rendp_info.pDependencies = &subp_dep;

  VkRenderPass rendp;
  vkCreateRenderPass(dev, &rendp_info, NULL, &rendp);
  printf("render pass created.\n");
  return (struct RenderPassGroup){rendp, 0, 0};
}

struct PipeGroup create_pipe(VkDevice dev, VkExtent2D extent,
                             VkRenderPass rendp) {
  //
  // load shader
  //
  FILE *fp_vert = NULL, *fp_frag = NULL;
  fp_vert = fopen("vert.spv", "rb+");
  fp_frag = fopen("frag.spv", "rb+");
  char shader_loaded = 1;
  if (fp_vert == NULL || fp_frag == NULL) {
    shader_loaded = 0;
    printf("cannot find shader code.\n");
  }
  fseek(fp_vert, 0, SEEK_END);
  fseek(fp_frag, 0, SEEK_END);
  uint32_t vert_size = ftell(fp_vert);
  uint32_t frag_size = ftell(fp_frag);

  char *p_vert_code = (char *)malloc(vert_size * sizeof(char));
  char *p_frag_code = (char *)malloc(frag_size * sizeof(char));

  rewind(fp_vert);
  rewind(fp_frag);
  fread(p_vert_code, 1, vert_size, fp_vert);
  printf("vertex shader binaries loaded.\n");
  fread(p_frag_code, 1, frag_size, fp_frag);
  printf("fragment shader binaries loaded.\n");

  fclose(fp_vert);
  fclose(fp_frag);
  //
  // create shader modules
  //
  VkShaderModuleCreateInfo vert_shad_mod_info;
  vert_shad_mod_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  vert_shad_mod_info.pNext = NULL;
  vert_shad_mod_info.flags = 0;
  vert_shad_mod_info.codeSize = shader_loaded ? vert_size : 0;
  vert_shad_mod_info.pCode =
      shader_loaded ? (const uint32_t *)p_vert_code : NULL;

  VkShaderModuleCreateInfo frag_shad_mod_info;
  frag_shad_mod_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  frag_shad_mod_info.pNext = NULL;
  frag_shad_mod_info.flags = 0;
  frag_shad_mod_info.codeSize = shader_loaded ? frag_size : 0;
  frag_shad_mod_info.pCode =
      shader_loaded ? (const uint32_t *)p_frag_code : NULL;

  VkShaderModule vert_shad_mod, frag_shad_mod;
  vkCreateShaderModule(dev, &vert_shad_mod_info, NULL, &vert_shad_mod);
  printf("vertex shader module created.\n");
  vkCreateShaderModule(dev, &frag_shad_mod_info, NULL, &frag_shad_mod);
  printf("fragment shader module created.\n");
  //
  // fill shader stage info
  //
  VkPipelineShaderStageCreateInfo vert_stage_info, frag_stage_info,
      stage_infos[2];

  vert_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  vert_stage_info.pNext = NULL;
  vert_stage_info.flags = 0;
  vert_stage_info.stage = VK_SHADER_STAGE_VERTEX_BIT;
  vert_stage_info.module = vert_shad_mod;
  char vert_entry[VK_MAX_EXTENSION_NAME_SIZE];
  strcpy(vert_entry, "main");
  vert_stage_info.pName = vert_entry;
  vert_stage_info.pSpecializationInfo = NULL;
  printf("vertex shader stage info filled.\n");

  frag_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  frag_stage_info.pNext = NULL;
  frag_stage_info.flags = 0;
  frag_stage_info.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
  frag_stage_info.module = frag_shad_mod;
  char frag_entry[VK_MAX_EXTENSION_NAME_SIZE];
  strcpy(frag_entry, "main");
  frag_stage_info.pName = frag_entry;
  frag_stage_info.pSpecializationInfo = NULL;
  printf("fragment shader stage info filled.\n");

  stage_infos[0] = vert_stage_info;
  stage_infos[1] = frag_stage_info;
  printf("created shader stages.\n");
  //
  // fill vertex input state info
  //
  VkPipelineVertexInputStateCreateInfo vert_input_info;
  vert_input_info.sType =
      VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
  vert_input_info.pNext = NULL;
  vert_input_info.flags = 0;
  vert_input_info.vertexBindingDescriptionCount = 0;
  //.vertexBindingDescriptionCount=1;
  vert_input_info.pVertexBindingDescriptions = NULL;
  //.pVertexBindingDescriptions=&vert_bindg_desc;
  vert_input_info.vertexAttributeDescriptionCount = 0;
  //.vertexAttributeDescriptionCount=1;
  vert_input_info.pVertexAttributeDescriptions = NULL;
  //.pVertexAttributeDescriptions=&vert_attr_desc;
  printf("vertex input state info filled.\n");
  //
  // fill input assembly state info
  //
  VkPipelineInputAssemblyStateCreateInfo input_asm_info;
  input_asm_info.sType =
      VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
  input_asm_info.pNext = NULL;
  input_asm_info.flags = 0;
  input_asm_info.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
  input_asm_info.primitiveRestartEnable = VK_FALSE;
  printf("input assembly info filled.\n");
  //
  // fill viewport
  //
  VkViewport viewport;
  viewport.x = 0.0f;
  viewport.y = 0.0f;
  viewport.width = (float)extent.width;
  viewport.height = (float)extent.height;
  viewport.minDepth = 0.0f;
  viewport.maxDepth = 1.0f;
  printf("viewport filled.\n");
  //
  // fill scissor
  //
  VkRect2D scissor;
  VkOffset2D sci_offset;
  sci_offset.x = 0;
  sci_offset.y = 0;
  scissor.offset = sci_offset;
  scissor.extent = extent;
  printf("scissor filled.\n");
  //
  // fill viewport state info
  //
  VkPipelineViewportStateCreateInfo vwp_state_info;
  vwp_state_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
  vwp_state_info.pNext = NULL;
  vwp_state_info.flags = 0;
  vwp_state_info.viewportCount = 1;
  vwp_state_info.pViewports = &viewport;
  vwp_state_info.scissorCount = 1;
  vwp_state_info.pScissors = &scissor;
  printf("viewport state filled.\n");
  //
  // fill rasterizer state info
  //
  VkPipelineRasterizationStateCreateInfo rast_info;
  rast_info.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
  rast_info.pNext = NULL;
  rast_info.flags = 0;
  rast_info.depthClampEnable = VK_FALSE;
  rast_info.rasterizerDiscardEnable = VK_FALSE;
  rast_info.polygonMode = VK_POLYGON_MODE_FILL;
  rast_info.cullMode = VK_CULL_MODE_BACK_BIT;
  rast_info.frontFace = VK_FRONT_FACE_CLOCKWISE;
  rast_info.depthBiasEnable = VK_FALSE;
  rast_info.depthBiasConstantFactor = 0.0f;
  rast_info.depthBiasClamp = 0.0f;
  rast_info.depthBiasSlopeFactor = 0.0f;
  rast_info.lineWidth = 1.0f;
  printf("rasterization info filled.\n");
  //
  // fill multisampling state info
  //
  VkPipelineMultisampleStateCreateInfo samp_info;
  samp_info.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
  samp_info.pNext = NULL;
  samp_info.flags = 0;
  samp_info.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
  samp_info.sampleShadingEnable = VK_FALSE;
  samp_info.minSampleShading = 1.0f;
  samp_info.pSampleMask = NULL;
  samp_info.alphaToCoverageEnable = VK_FALSE;
  samp_info.alphaToOneEnable = VK_FALSE;
  printf("multisample info filled.\n");
  //
  // fill color blend attachment state
  //
  VkPipelineColorBlendAttachmentState color_blend_attach;
  color_blend_attach.blendEnable = VK_FALSE;
  color_blend_attach.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
  color_blend_attach.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
  color_blend_attach.colorBlendOp = VK_BLEND_OP_ADD;
  color_blend_attach.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
  color_blend_attach.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
  color_blend_attach.alphaBlendOp = VK_BLEND_OP_ADD;
  color_blend_attach.colorWriteMask =
      VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
      VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
  printf("color blend attachment state filled.\n");
  //
  // fill color blend state info
  //
  VkPipelineColorBlendStateCreateInfo color_blend_info;
  color_blend_info.sType =
      VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
  color_blend_info.pNext = NULL;
  color_blend_info.flags = 0;
  color_blend_info.logicOpEnable = VK_FALSE;
  color_blend_info.logicOp = VK_LOGIC_OP_COPY;
  color_blend_info.attachmentCount = 1;
  color_blend_info.pAttachments = &color_blend_attach;
  for (uint32_t i = 0; i < 4; i++) {
    color_blend_info.blendConstants[i] = 0.0f;
  }
  printf("color blend state info filled.\n");
  //
  // create pipeline layout
  //
  VkPipelineLayoutCreateInfo pipe_layout_info;
  pipe_layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  pipe_layout_info.pNext = NULL;
  pipe_layout_info.flags = 0;
  pipe_layout_info.setLayoutCount = 0;
  pipe_layout_info.pSetLayouts = NULL;
  pipe_layout_info.pushConstantRangeCount = 0;
  pipe_layout_info.pPushConstantRanges = NULL;

  VkPipelineLayout pipe_layout;
  vkCreatePipelineLayout(dev, &pipe_layout_info, NULL, &pipe_layout);
  printf("pipeline layout created.\n");
  //
  // create pipeline
  //
  VkGraphicsPipelineCreateInfo pipe_info;
  pipe_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
  pipe_info.pNext = NULL;
  pipe_info.flags = 0;
  pipe_info.stageCount = 2;
  pipe_info.pStages = stage_infos;
  pipe_info.pVertexInputState = &vert_input_info;
  pipe_info.pInputAssemblyState = &input_asm_info;
  pipe_info.pTessellationState = NULL;
  pipe_info.pViewportState = &vwp_state_info;
  pipe_info.pRasterizationState = &rast_info;
  pipe_info.pMultisampleState = &samp_info;
  pipe_info.pDepthStencilState = NULL;
  pipe_info.pColorBlendState = &color_blend_info;
  pipe_info.pDynamicState = NULL;

  pipe_info.layout = pipe_layout;
  pipe_info.renderPass = rendp;
  pipe_info.subpass = 0;
  pipe_info.basePipelineHandle = VK_NULL_HANDLE;
  pipe_info.basePipelineIndex = -1;

  VkPipeline pipe;
  vkCreateGraphicsPipelines(dev, VK_NULL_HANDLE, 1, &pipe_info, NULL, &pipe);
  printf("graphics pipeline created.\n");
  //
  // destroy shader module
  //
  vkDestroyShaderModule(dev, frag_shad_mod, NULL);
  printf("fragment shader module destroyed.\n");
  vkDestroyShaderModule(dev, vert_shad_mod, NULL);
  printf("vertex shader module destroyed.\n");
  free(p_frag_code);
  printf("fragment shader binaries released.\n");
  free(p_vert_code);
  printf("vertex shader binaries released.\n");
  return (struct PipeGroup){pipe, pipe_layout};
}

void create_frame_buffs(struct RenderPassGroup *g_rendp, VkDevice dev,
                        uint32_t img_count, VkImageView *img_views,
                        VkExtent2D extent) {
  //
  // create framebuffer
  //
  VkFramebufferCreateInfo frame_buff_infos[img_count];
  VkFramebuffer frame_buffs[img_count];
  VkImageView img_attachs[img_count];
  for (uint32_t i = 0; i < img_count; i++) {
    img_attachs[i] = img_views[i];
    frame_buff_infos[i].sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    frame_buff_infos[i].pNext = NULL;
    frame_buff_infos[i].flags = 0;
    frame_buff_infos[i].renderPass = g_rendp->rendp;
    frame_buff_infos[i].attachmentCount = 1;
    frame_buff_infos[i].pAttachments = &(img_attachs[i]);
    frame_buff_infos[i].width = extent.width;
    frame_buff_infos[i].height = extent.height;
    frame_buff_infos[i].layers = 1;

    vkCreateFramebuffer(dev, &(frame_buff_infos[i]), NULL, &(frame_buffs[i]));
    printf("framebuffer %d created.\n", i);
  }
  g_rendp->frame_buff_count = img_count;
  g_rendp->frame_buffs = frame_buffs[0];
}

void render_frame(VkDevice dev, VkSwapchainKHR swap,
                  struct RenderMgmtGroup *rend_mgmt, VkQueue q_graph,
                  VkQueue q_pres) {
  vkWaitForFences(dev, 1, &(rend_mgmt->fens[rend_mgmt->cur_frame]), VK_TRUE,
                  UINT64_MAX);

  uint32_t img_index = 0;
  vkAcquireNextImageKHR(dev, swap, UINT64_MAX,
                        rend_mgmt->img_avl_semps[rend_mgmt->cur_frame],
                        VK_NULL_HANDLE, &img_index);

  if (rend_mgmt->img_fens[img_index] != VK_NULL_HANDLE) {
    vkWaitForFences(dev, 1, &(rend_mgmt->img_fens[img_index]), VK_TRUE,
                    UINT64_MAX);
  }

  rend_mgmt->img_fens[img_index] = rend_mgmt->fens[rend_mgmt->cur_frame];

  VkSubmitInfo sub_info;
  sub_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  sub_info.pNext = NULL;

  VkSemaphore semps_wait[1];
  semps_wait[0] = rend_mgmt->img_avl_semps[rend_mgmt->cur_frame];
  VkPipelineStageFlags wait_stages[1];
  wait_stages[0] = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

  sub_info.waitSemaphoreCount = 1;
  sub_info.pWaitSemaphores = &(semps_wait[0]);
  sub_info.pWaitDstStageMask = &(wait_stages[0]);
  sub_info.commandBufferCount = 1;
  sub_info.pCommandBuffers = &(rend_mgmt->cmd_buffs[img_index]);

  VkSemaphore semps_sig[1];
  semps_sig[0] = rend_mgmt->rend_fin_semps[rend_mgmt->cur_frame];

  sub_info.signalSemaphoreCount = 1;
  sub_info.pSignalSemaphores = &(semps_sig[0]);

  vkResetFences(dev, 1, &(rend_mgmt->fens[rend_mgmt->cur_frame]));

  vkQueueSubmit(q_graph, 1, &sub_info, rend_mgmt->fens[rend_mgmt->cur_frame]);
  //
  // present
  //
  VkPresentInfoKHR pres_info;

  pres_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
  pres_info.pNext = NULL;
  pres_info.waitSemaphoreCount = 1;
  pres_info.pWaitSemaphores = &(semps_sig[0]);

  VkSwapchainKHR swaps[1];
  swaps[0] = swap;
  pres_info.swapchainCount = 1;
  pres_info.pSwapchains = &(swaps[0]);
  pres_info.pImageIndices = &img_index;
  pres_info.pResults = NULL;

  vkQueuePresentKHR(q_pres, &pres_info);

  rend_mgmt->cur_frame = (rend_mgmt->cur_frame + 1) % rend_mgmt->max_frames;
}

void destroy_render(struct DevGroup *dev, struct PipeGroup *pipe,
                    struct RenderPassGroup *rendp) {
  //
  // destroy frambuffer
  //
  for (uint32_t i = 0; i < rendp->frame_buff_count; i++) {
    vkDestroyFramebuffer(dev->dev, rendp->frame_buffs[i], NULL);
    printf("frambuffer %d destroyed.\n", i);
  }
  //
  // destroy pipeline
  //
  vkDestroyPipeline(dev->dev, pipe->pipe, NULL);
  printf("graphics pipeline destroyed.\n");
  //
  // destroy pipeline layout
  //
  vkDestroyPipelineLayout(dev->dev, pipe->pipe_layout, NULL);
  printf("pipeline layout destroyed.\n");
  //
  // destroy render pass
  //
  vkDestroyRenderPass(dev->dev, rendp, NULL);
  printf("render pass destroyed.\n");
}