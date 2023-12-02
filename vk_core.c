#include "vk_core.h"

const VkComponentMapping DEFAULT_COMPONENT_MAPPING = {
    .r = VK_COMPONENT_SWIZZLE_IDENTITY,
    .g = VK_COMPONENT_SWIZZLE_IDENTITY,
    .b = VK_COMPONENT_SWIZZLE_IDENTITY,
    .a = VK_COMPONENT_SWIZZLE_IDENTITY};

const VkImageSubresourceRange DEFAULT_SUBRESOURCE_RANGE = {
    .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
    .baseMipLevel = 0,
    .levelCount = 1,
    .baseArrayLayer = 0,
    .layerCount = 1};

VkApplicationInfo create_app_info() {
  //
  // create application info
  //
  VkApplicationInfo app_info;

  app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  app_info.pNext = NULL;
  char app_name[VK_MAX_EXTENSION_NAME_SIZE];
  strcpy_s(app_name, sizeof(app_name), "vulkan_project");
  app_info.pApplicationName = app_name;
  app_info.applicationVersion = VK_MAKE_VERSION(0, 0, 1);
  char app_engine_name[VK_MAX_EXTENSION_NAME_SIZE];
  strcpy_s(app_engine_name, sizeof(app_engine_name), "vulkan_engine");
  app_info.pEngineName = app_engine_name;
  app_info.engineVersion = VK_MAKE_VERSION(0, 0, 1);
  app_info.apiVersion = VK_API_VERSION_1_3;

  return app_info;
}

VkInstance create_inst(VkApplicationInfo *app_info) {
  glfwInit();
  //
  // create instance create info
  //
  VkInstanceCreateInfo inst_info;

  inst_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  inst_info.pNext = NULL;
  inst_info.flags = 0;
  inst_info.pApplicationInfo = app_info;
  uint32_t layer_count = 1;
  inst_info.enabledLayerCount = layer_count;
  char pp_layers[layer_count][VK_MAX_EXTENSION_NAME_SIZE];
  strcpy_s(pp_layers[0], sizeof(pp_layers[0]), "VK_LAYER_KHRONOS_validation");
  char *pp_layer_names[layer_count];
  for (uint32_t i = 0; i < layer_count; i++) {
    pp_layer_names[i] = pp_layers[i];
  }
  inst_info.ppEnabledLayerNames = (const char *const *)pp_layer_names;
  uint32_t ext_count = 0;
  const char *const *pp_ext_names =
      glfwGetRequiredInstanceExtensions(&ext_count);
  inst_info.enabledExtensionCount = ext_count;
  inst_info.ppEnabledExtensionNames = pp_ext_names;
  //
  // create instance
  //
  VkInstance inst;

  vkCreateInstance(&inst_info, NULL, &inst);
  printf("instance created.\n");
  return inst;
}

void check_phy_dev(VkPhysicalDevice phy_dev) {
  VkPhysicalDeviceProperties props;
  vkGetPhysicalDeviceProperties(phy_dev, &props);
  printf("valid device with name: %s\n", props.deviceName);
}

struct PhyDevGroup get_phy_dev(VkInstance inst) {
  //
  // enumerate physical devices
  //
  uint32_t phy_dev_count = 0;
  vkEnumeratePhysicalDevices(inst, &phy_dev_count, NULL);

  VkPhysicalDevice phy_devs[phy_dev_count];
  vkEnumeratePhysicalDevices(inst, &phy_dev_count, phy_devs);
  //
  // select physical device
  //
  VkPhysicalDeviceProperties phy_dev_props[phy_dev_count];
  uint32_t discrete_gpu_list[phy_dev_count];
  uint32_t discrete_gpu_count = 0;
  uint32_t intergrated_gpu_list[phy_dev_count];
  uint32_t intergrated_gpu_count = 0;

  VkPhysicalDeviceMemoryProperties phy_dev_mem_props[phy_dev_count];
  uint32_t phy_dev_mem_count[phy_dev_count];
  VkDeviceSize phy_dev_mem_total[phy_dev_count];

  for (uint32_t i = 0; i < phy_dev_count; i++) {
    vkGetPhysicalDeviceProperties(phy_devs[i], &phy_dev_props[i]);
    if (phy_dev_props[i].deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
      discrete_gpu_list[discrete_gpu_count] = i;
      discrete_gpu_count++;
    } else if (phy_dev_props[i].deviceType ==
               VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU) {
      intergrated_gpu_list[intergrated_gpu_count] = i;
      intergrated_gpu_count++;
    }

    vkGetPhysicalDeviceMemoryProperties(phy_devs[i], &phy_dev_mem_props[i]);
    phy_dev_mem_count[i] = phy_dev_mem_props[i].memoryHeapCount;
    phy_dev_mem_total[i] = 0;
    for (uint32_t j = 0; j < phy_dev_mem_count[i]; j++) {
      phy_dev_mem_total[i] += phy_dev_mem_props[i].memoryHeaps[j].size;
    }
  }

  VkDeviceSize max_mem_size = 0;
  uint32_t best_idx = 0;

  if (discrete_gpu_count != 0) {
    for (uint32_t i = 0; i < discrete_gpu_count; i++) {
      if (phy_dev_mem_total[i] > max_mem_size) {
        best_idx = discrete_gpu_list[i];
        max_mem_size = phy_dev_mem_total[i];
      }
    }
  } else if (intergrated_gpu_count != 0) {
    for (uint32_t i = 0; i < intergrated_gpu_count; i++) {
      if (phy_dev_mem_total[i] > max_mem_size) {
        best_idx = intergrated_gpu_list[i];
        max_mem_size = phy_dev_mem_total[i];
      }
    }
  }

  printf("best device index: %u\n", best_idx);
  printf("device name: %s\n", phy_dev_props[best_idx].deviceName);
  printf("device type: ");
  if (discrete_gpu_count != 0) {
    printf("discrete gpu\n");
  } else if (intergrated_gpu_count != 0) {
    printf("intergrated gpu\n");
  } else {
    printf("unknown\n");
  }
  printf("memory total: %llu\n", phy_dev_mem_total[best_idx]);

  return (struct PhyDevGroup){phy_devs[best_idx], phy_dev_props[best_idx],
                              phy_dev_mem_props[best_idx],
                              phy_dev_mem_total[best_idx]};
}

struct DevGroup create_dev(VkPhysicalDevice phy_dev) {
  //
  // query queue families
  //
  uint32_t qf_prop_count = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(phy_dev, &qf_prop_count, NULL);
  VkQueueFamilyProperties qf_props[qf_prop_count];
  vkGetPhysicalDeviceQueueFamilyProperties(phy_dev, &qf_prop_count, qf_props);

  printf("found queue families:\n");
  uint32_t qf_q_count[qf_prop_count];
  for (uint32_t i = 0; i < qf_prop_count; i++) {
    qf_q_count[i] = qf_props[i].queueCount;
    printf("  index: %x count: %x\n", i, qf_props[i].queueCount);
  }

  //
  // create logical device
  //
  VkDeviceQueueCreateInfo dev_q_infos[qf_prop_count];
  for (uint32_t i = 0; i < qf_prop_count; i++) {
    dev_q_infos[i].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    dev_q_infos[i].pNext = NULL;
    dev_q_infos[i].flags = 0;
    dev_q_infos[i].queueFamilyIndex = i;
    dev_q_infos[i].queueCount = qf_q_count[i];
    float q_prior[1] = {1.0f};
    dev_q_infos[i].pQueuePriorities = q_prior;
  }
  printf("using %d queue families.\n", qf_prop_count);

  VkDeviceCreateInfo dev_info;
  dev_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
  dev_info.pNext = NULL;
  dev_info.flags = 0;
  dev_info.queueCreateInfoCount = qf_prop_count;
  dev_info.pQueueCreateInfos = dev_q_infos;
  dev_info.enabledLayerCount = 0;
  dev_info.ppEnabledLayerNames = NULL;

  uint32_t ext_count = 1;
  dev_info.enabledExtensionCount = ext_count;
  char pp_exts[ext_count][VK_MAX_EXTENSION_NAME_SIZE];
  strcpy_s(pp_exts[0], sizeof(pp_exts[0]), "VK_KHR_swapchain");
  printf("using extensions:\n");
  char *pp_ext_names[ext_count];
  for (uint32_t i = 0; i < ext_count; i++) {
    pp_ext_names[i] = pp_exts[i];
    printf("  %s\n", pp_ext_names[i]);
  }
  dev_info.ppEnabledExtensionNames = (const char *const *)pp_ext_names;
  VkPhysicalDeviceFeatures phy_dev_feat;
  vkGetPhysicalDeviceFeatures(phy_dev, &phy_dev_feat);
  dev_info.pEnabledFeatures = &phy_dev_feat;

  VkDevice dev;
  vkCreateDevice(phy_dev, &dev_info, NULL, &dev);
  printf("logical device created.\n");
  //
  // select best queue
  //
  uint32_t qf_graph_count = 0;
  uint32_t qf_graph_list[qf_prop_count];
  for (uint32_t i = 0; i < qf_prop_count; i++) {
    if ((qf_props[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0) {
      qf_graph_list[qf_graph_count] = i;
      qf_graph_count++;
    }
  }

  uint32_t max_q_count = 0;
  uint32_t qf_best_idx = 0;
  for (uint32_t i = 0; i < qf_graph_count; i++) {
    if (qf_props[qf_graph_list[i]].queueCount > max_q_count) {
      qf_best_idx = qf_graph_list[i];
    }
  }
  printf("best queue family index: %d\n", qf_best_idx);

  VkQueue q_graph, q_pres;
  vkGetDeviceQueue(dev, qf_best_idx, 0, &q_graph);
  char single_queue = 1;
  if (qf_props[qf_best_idx].queueCount < 2) {
    vkGetDeviceQueue(dev, qf_best_idx, 0, &q_pres);
    printf("using single queue for drawing.\n");
  } else {
    single_queue = 0;
    vkGetDeviceQueue(dev, qf_best_idx, 1, &q_pres);
    printf("using double queues for drawing.\n");
  }

  return (struct DevGroup){dev, qf_best_idx, q_graph, q_pres, single_queue};
}

static void cursor_position_callback(GLFWwindow *window, double x, double y) {
  printf("cursor position xpos:%f ypos:%f\n", x, y);
}

struct SurfaceGroup create_surf(VkInstance inst, VkPhysicalDevice phy_dev,
                                size_t qf_best_idx, VkExtent2D dim) {
  //
  // create window and surface
  //
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  glfwWindowHint(GLFW_RESIZABLE,
                 GLFW_FALSE); // TODO: support dynamic window size
  GLFWwindow *window = glfwCreateWindow(dim.width, dim.height, "", NULL, NULL);
  printf("window created.\n");
  VkSurfaceKHR surf;
  glfwCreateWindowSurface(inst, window, NULL, &surf);
  printf("surface created.\n");
  glfwSetCursorPosCallback(window, cursor_position_callback);
  printf("cursor position callback created.\n");
  //
  // verify surface support
  //
  VkBool32 supported;
  vkGetPhysicalDeviceSurfaceSupportKHR(phy_dev, qf_best_idx, surf, &supported);
  if (supported == VK_TRUE) {
    printf("surface supported.\n");
  } else {
    printf("warning: surface unsupported!\n");
  }
  //
  // fetch surface capabilities
  //
  VkSurfaceCapabilitiesKHR caps;
  vkGetPhysicalDeviceSurfaceCapabilitiesKHR(phy_dev, surf, &caps);
  printf("fetched caps from surface.\n");
  char extent_suitable = 1;
  int wind_w, wind_h;
  glfwGetFramebufferSize(window, &wind_w, &wind_h);
  VkExtent2D actual_extent;
  actual_extent.width = wind_w;
  actual_extent.height = wind_h;
  if (caps.currentExtent.width != wind_w ||
      caps.currentExtent.height != wind_h) {
    extent_suitable = 0;
    printf("actual extent size doesn't match framebuffers, resizing...\n");
    actual_extent.width =
        wind_w > caps.maxImageExtent.width ? caps.maxImageExtent.width : wind_w;
    actual_extent.width =
        wind_w < caps.minImageExtent.width ? caps.minImageExtent.width : wind_w;
    actual_extent.height = wind_h > caps.maxImageExtent.height
                               ? caps.maxImageExtent.height
                               : wind_h;
    actual_extent.height = wind_h < caps.minImageExtent.height
                               ? caps.minImageExtent.height
                               : wind_h;
  }
  //
  // fetch surface formats
  //
  uint32_t form_count;
  vkGetPhysicalDeviceSurfaceFormatsKHR(phy_dev, surf, &form_count, NULL);
  VkSurfaceFormatKHR *forms = malloc(form_count * sizeof(VkSurfaceFormatKHR));
  vkGetPhysicalDeviceSurfaceFormatsKHR(phy_dev, surf, &form_count, forms);
  printf("fetched %d surface formats.\n", form_count);
  for (uint32_t i = 0; i < form_count; i++) {
    printf("format: %d\tcolorspace: %d\n", forms[i].format, forms[i].colorSpace);
  }
  //
  // fetch surface present mode
  // TODO: add dynamic present mode support
  //
  uint32_t pres_mode_count;
  vkGetPhysicalDeviceSurfacePresentModesKHR(phy_dev, surf, &pres_mode_count,
                                            NULL);
  VkPresentModeKHR pres_modes[pres_mode_count];
  vkGetPhysicalDeviceSurfacePresentModesKHR(phy_dev, surf, &pres_mode_count,
                                            pres_modes);
  printf("fetched %d present modes.\n", pres_mode_count);
  char mailbox_mode_supported = 0;
  for (uint32_t i = 0; i < pres_mode_count; i++) {
    printf("present mode: %d\n", pres_modes[i]);
    if (pres_modes[i] == VK_PRESENT_MODE_MAILBOX_KHR) {
      printf("mailbox present mode supported.\n");
      mailbox_mode_supported = 1;
    }
  }

  return (struct SurfaceGroup){.window = window,
                               .surf = surf,
                               .supported = supported,
                               .forms = forms,
                               .form_count = form_count,
                               .mailbox_mode_supported = mailbox_mode_supported,
                               .caps = caps,
                               .extent_suitable = extent_suitable,
                               .actual_extent = actual_extent};
}

struct SwapchainGroup create_swap(VkDevice dev, VkSwapchainKHR old,
                                  VkSurfaceKHR surf,
                                  VkSurfaceCapabilitiesKHR caps,
                                  VkSurfaceFormatKHR form, char extent_suitable,
                                  VkExtent2D actual_extent, char single_queue,
                                  char mailbox_mode_supported) {
  //
  // create swapchain
  //
  VkSwapchainCreateInfoKHR swap_info;
  swap_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
  swap_info.pNext = NULL;
  swap_info.flags = 0;
  swap_info.surface = surf;
  swap_info.minImageCount = caps.minImageCount + 1;
  swap_info.imageFormat = form.format;
  swap_info.imageColorSpace = form.colorSpace;
  swap_info.imageExtent = extent_suitable ? caps.currentExtent : actual_extent;
  swap_info.imageArrayLayers = 1;
  swap_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
  swap_info.imageSharingMode =
      single_queue ? VK_SHARING_MODE_EXCLUSIVE : VK_SHARING_MODE_CONCURRENT;
  swap_info.queueFamilyIndexCount = single_queue ? 0 : 2;
  uint32_t qf_indices[2] = {0, 1};
  swap_info.pQueueFamilyIndices = single_queue ? NULL : qf_indices;
  swap_info.preTransform = caps.currentTransform;
  swap_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
  swap_info.presentMode = mailbox_mode_supported ? VK_PRESENT_MODE_MAILBOX_KHR
                                                 : VK_PRESENT_MODE_FIFO_KHR;
  swap_info.clipped = VK_TRUE;
  swap_info.oldSwapchain = old;

  VkSwapchainKHR swap;
  vkCreateSwapchainKHR(dev, &swap_info, NULL, &swap);
  printf("swapchain created.\n");
  //
  // fetch image from swapchain
  //
  uint32_t img_count = 0;
  vkGetSwapchainImagesKHR(dev, swap, &img_count, NULL);
  VkImage *imgs = malloc(img_count * sizeof(VkImage));
  vkGetSwapchainImagesKHR(dev, swap, &img_count, imgs);
  printf("%d images fetched from swapchain.\n", img_count);
  //
  // create image view
  //
  VkImageView *img_views = malloc(img_count * sizeof(VkImageView));
  VkImageViewCreateInfo img_view_infos[img_count];

  VkImageSubresourceRange subres;
  subres = DEFAULT_SUBRESOURCE_RANGE;
  subres.layerCount = swap_info.imageArrayLayers;

  for (uint32_t i = 0; i < img_count; i++) {
    img_view_infos[i].sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    img_view_infos[i].pNext = NULL;
    img_view_infos[i].flags = 0;
    img_view_infos[i].image = imgs[i];
    img_view_infos[i].viewType = VK_IMAGE_VIEW_TYPE_2D;
    img_view_infos[i].format = form.format;
    img_view_infos[i].components = DEFAULT_COMPONENT_MAPPING;
    img_view_infos[i].subresourceRange = subres;
    vkCreateImageView(dev, &img_view_infos[i], NULL, &img_views[i]);
    printf("image view %d created.\n", i);
  }

  return (struct SwapchainGroup){swap, swap_info.imageExtent, img_count, imgs,
                                 img_views};
}

struct RenderMgmtGroup create_rend_mgmt(VkDevice dev, uint32_t qf_best_idx,
                                        uint32_t img_count, VkExtent2D extent,
                                        VkRenderPass rendp,
                                        VkFramebuffer *frame_buffs,
                                        VkPipeline pipe) {
  //
  // create command pool
  //
  VkCommandPoolCreateInfo cmd_pool_info;
  cmd_pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  cmd_pool_info.pNext = NULL;
  cmd_pool_info.flags = 0;
  cmd_pool_info.queueFamilyIndex = qf_best_idx;

  VkCommandPool cmd_pool;
  vkCreateCommandPool(dev, &cmd_pool_info, NULL, &cmd_pool);
  printf("command pool created.\n");
  //
  // allocate command buffers
  //
  VkCommandBufferAllocateInfo cmd_alloc_info;
  cmd_alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  cmd_alloc_info.pNext = NULL;
  cmd_alloc_info.commandPool = cmd_pool;
  cmd_alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  cmd_alloc_info.commandBufferCount = img_count;

  VkCommandBuffer *cmd_buffs = malloc(img_count * sizeof(VkCommandBuffer));
  vkAllocateCommandBuffers(dev, &cmd_alloc_info, cmd_buffs);
  printf("command buffers allocated.\n");
  //
  // render preparation
  //
  VkCommandBufferBeginInfo *cmd_begin_infos = malloc(img_count * sizeof(VkCommandBufferBeginInfo));
  VkRenderPassBeginInfo *rendp_begin_infos = malloc(img_count * sizeof(VkRenderPassBeginInfo));
  VkRect2D rendp_area;
  rendp_area.offset.x = 0;
  rendp_area.offset.y = 0;
  rendp_area.extent = extent;
  VkClearValue clear_val = {0.6f, 0.2f, 0.8f, 0.0f};
  for (uint32_t i = 0; i < img_count; i++) {

    cmd_begin_infos[i].sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    cmd_begin_infos[i].pNext = NULL;
    cmd_begin_infos[i].flags = 0;
    cmd_begin_infos[i].pInheritanceInfo = NULL;
    printf("command buffer begin info %d filled.\n", i);

    rendp_begin_infos[i].sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    rendp_begin_infos[i].pNext = NULL;
    rendp_begin_infos[i].renderPass = rendp;
    rendp_begin_infos[i].framebuffer = frame_buffs[i];
    rendp_begin_infos[i].renderArea = rendp_area;
    rendp_begin_infos[i].clearValueCount = 1;
    rendp_begin_infos[i].pClearValues = &clear_val;
    printf("render pass begin info %d filled.\n", i);

    vkBeginCommandBuffer(cmd_buffs[i], &cmd_begin_infos[i]);

    vkCmdBeginRenderPass(cmd_buffs[i], &(rendp_begin_infos[i]),
                         VK_SUBPASS_CONTENTS_INLINE);

    vkCmdBindPipeline(cmd_buffs[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipe);

    vkCmdDraw(cmd_buffs[i], 3, 1, 0, 0);

    vkCmdEndRenderPass(cmd_buffs[i]);

    vkEndCommandBuffer(cmd_buffs[i]);

    printf("command buffer drawing recorded.\n");
  }
  //
  // semaphore and fence creation
  //
  // THESE ARE MAX FRAMES IN FLIGHT
  uint32_t max_frames = 2;
  VkSemaphore *img_avl_semps = malloc(max_frames * sizeof(VkSemaphore));
  VkSemaphore *rend_fin_semps = malloc(max_frames * sizeof(VkSemaphore));
  VkFence *fens = malloc(max_frames * sizeof(VkFence));

  VkSemaphoreCreateInfo semp_info;
  semp_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
  semp_info.pNext = NULL;
  semp_info.flags = 0;

  VkFenceCreateInfo fen_info;
  fen_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
  fen_info.pNext = NULL;
  fen_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

  for (uint32_t i = 0; i < max_frames; i++) {
    vkCreateSemaphore(dev, &semp_info, NULL, &(img_avl_semps[i]));
    vkCreateSemaphore(dev, &semp_info, NULL, &(rend_fin_semps[i]));
    vkCreateFence(dev, &fen_info, NULL, &(fens[i]));
  }
  printf("semaphores and fences created.\n");

  uint32_t cur_frame = 0;
  VkFence *img_fens = malloc(max_frames * sizeof(VkFence));
  for (uint32_t i = 0; i < img_count; i++) {
    img_fens[i] = VK_NULL_HANDLE;
  }

  return (struct RenderMgmtGroup){cmd_pool,       cmd_buffs,  img_fens,
                                  img_count,      max_frames, img_avl_semps,
                                  rend_fin_semps, fens,       cur_frame};
}

void destroy_vk_core(VkInstance inst, struct DevGroup *dev,
                     struct SwapchainGroup *swap, struct SurfaceGroup *surf,
                     struct RenderMgmtGroup *rend_mgmt) {
  //
  // free command buffer
  //
  vkFreeCommandBuffers(dev->dev, rend_mgmt->cmd_pool, rend_mgmt->img_count,
                       rend_mgmt->cmd_buffs);
  printf("command buffers freed.\n");
  //
  // destroy semaphores and fences
  //
  for (uint32_t i = 0; i < rend_mgmt->max_frames; i++) {
    vkDestroySemaphore(dev->dev, rend_mgmt->img_avl_semps[i], NULL);
    vkDestroySemaphore(dev->dev, rend_mgmt->rend_fin_semps[i], NULL);
    vkDestroyFence(dev->dev, rend_mgmt->fens[i], NULL);
  }
  printf("semaphores and fences destroyed.\n");
  //
  // destroy command pool
  //
  vkDestroyCommandPool(dev->dev, rend_mgmt->cmd_pool, NULL);
  printf("command pool destroyed.\n");
  //
  // destroy imageview
  //
  for (uint32_t i = 0; i < swap->img_count; i++) {
    vkDestroyImageView(dev->dev, swap->img_views[i], NULL);
    printf("image view %d destroyed.\n", i);
  }
  //
  // destroy swapchain
  //
  vkDestroySwapchainKHR(dev->dev, swap->swap, NULL);
  printf("swapchain destroyed.\n");
  //
  // destroy surface and window
  //
  vkDestroySurfaceKHR(inst, surf->surf, NULL);
  printf("surface destroyed.\n");
  glfwDestroyWindow(surf->window);
  printf("window destroyed.\n");
  //
  // destroy device
  //
  vkDestroyDevice(dev->dev, NULL);
  printf("logical device destroyed.\n");
  //
  // destroy instance
  //
  vkDestroyInstance(inst, NULL);
  printf("instance destroyed.\n");

  glfwTerminate();
}
