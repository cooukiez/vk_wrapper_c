#include "vk_core.h"

#define ENABLED_LAYER_COUNT 1
#define ENABLED_DEV_EXT_COUNT 1

const VkComponentMapping DEFAULT_COMPONENT_MAPPING = {.r = VK_COMPONENT_SWIZZLE_IDENTITY, .g = VK_COMPONENT_SWIZZLE_IDENTITY, .b = VK_COMPONENT_SWIZZLE_IDENTITY, .a = VK_COMPONENT_SWIZZLE_IDENTITY};

const VkImageSubresourceRange DEFAULT_SUBRESOURCE_RANGE = {.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT, .baseMipLevel = 0, .levelCount = 1, .baseArrayLayer = 0, .layerCount = 1};

VCW_PhysicalDevice *VCW_PHY_DEV = NULL;
VCW_Device *VCW_DEV = NULL;
VCW_Surface *VCW_SURF = NULL;
VCW_Swapchain *VCW_SWAP = NULL;

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
    inst_info.enabledLayerCount = ENABLED_LAYER_COUNT;
    char pp_layers[ENABLED_LAYER_COUNT][VK_MAX_EXTENSION_NAME_SIZE];

    strcpy_s(pp_layers[0], sizeof(pp_layers[0]), "VK_LAYER_KHRONOS_validation");

    char *pp_layer_names[ENABLED_LAYER_COUNT];
    for (uint32_t i = 0; i < ENABLED_LAYER_COUNT; i++) {
        pp_layer_names[i] = pp_layers[i];
    }
    inst_info.ppEnabledLayerNames = (const char *const *) pp_layer_names;
    uint32_t ext_count = 0;
    const char *const *pp_ext_names = glfwGetRequiredInstanceExtensions(&ext_count);
    inst_info.enabledExtensionCount = ext_count;
    inst_info.ppEnabledExtensionNames = pp_ext_names;
    //
    // create instance
    //
    VkInstance instance;
    vkCreateInstance(&inst_info, NULL, &instance);
    printf("instance created.\n");

    return instance;
}

void check_phy_dev(VkPhysicalDevice phy_dev) {
    VkPhysicalDeviceProperties props;
    vkGetPhysicalDeviceProperties(phy_dev, &props);
    printf("valid device with name: %s\n", props.deviceName);
}

VCW_PhysicalDevice *get_phy_dev(VkInstance inst) {
    VCW_PHY_DEV = malloc(sizeof(VCW_PhysicalDevice));
    //
    // enumerate physical devices
    //
    uint32_t phy_dev_count = 0;
    vkEnumeratePhysicalDevices(inst, &phy_dev_count, NULL);

    VkPhysicalDevice *phy_devs = malloc(phy_dev_count * sizeof(VkPhysicalDevice));
    vkEnumeratePhysicalDevices(inst, &phy_dev_count, phy_devs);
    //
    // select physical device
    //
    VkPhysicalDeviceProperties *phy_dev_props = malloc(phy_dev_count * sizeof(VkPhysicalDeviceProperties));
    uint32_t *discrete_gpu_list = malloc(phy_dev_count * sizeof(uint32_t));
    uint32_t discrete_gpu_count = 0;
    uint32_t *integrated_gpu_list = malloc(phy_dev_count * sizeof(uint32_t));
    uint32_t integrated_gpu_count = 0;

    VkPhysicalDeviceMemoryProperties *phy_dev_mem_props = malloc(
            phy_dev_count * sizeof(VkPhysicalDeviceMemoryProperties));
    uint32_t *phy_dev_mem_count = malloc(phy_dev_count * sizeof(uint32_t));
    VkDeviceSize *phy_dev_mem_total = malloc(phy_dev_count * sizeof(VkDeviceSize));

    for (uint32_t i = 0; i < phy_dev_count; i++) {
        vkGetPhysicalDeviceProperties(phy_devs[i], &phy_dev_props[i]);
        if (phy_dev_props[i].deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
            discrete_gpu_list[discrete_gpu_count] = i;
            discrete_gpu_count++;
        } else if (phy_dev_props[i].deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU) {
            integrated_gpu_list[integrated_gpu_count] = i;
            integrated_gpu_count++;
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
    } else if (integrated_gpu_count != 0) {
        for (uint32_t i = 0; i < integrated_gpu_count; i++) {
            if (phy_dev_mem_total[i] > max_mem_size) {
                best_idx = integrated_gpu_list[i];
                max_mem_size = phy_dev_mem_total[i];
            }
        }
    }

    printf("best device index: %u\n", best_idx);
    printf("device name: %s\n", phy_dev_props[best_idx].deviceName);
    printf("device type: ");
    if (discrete_gpu_count != 0) {
        printf("discrete gpu\n");
    } else if (integrated_gpu_count != 0) {
        printf("intergrated gpu\n");
    } else {
        printf("unknown\n");
    }
    printf("memory total: %llu\n", phy_dev_mem_total[best_idx]);

    VCW_PHY_DEV->dev = phy_devs[best_idx];
    VCW_PHY_DEV->prop = phy_dev_props[best_idx];
    VCW_PHY_DEV->mem_prop = phy_dev_mem_props[best_idx];
    VCW_PHY_DEV->mem_total = phy_dev_mem_total[best_idx];

    return VCW_PHY_DEV;
}

VCW_Device *create_dev(VCW_PhysicalDevice *vcw_phy_dev) {
    VCW_DEV = malloc(sizeof(VCW_Device));
    //
    // query queue families
    //
    uint32_t qf_prop_count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(vcw_phy_dev->dev, &qf_prop_count, NULL);
    VkQueueFamilyProperties *qf_props = malloc(qf_prop_count * sizeof(VkQueueFamilyProperties));
    vkGetPhysicalDeviceQueueFamilyProperties(vcw_phy_dev->dev, &qf_prop_count, qf_props);

    printf("found queue families:\n");
    uint32_t *qf_q_count = malloc(qf_prop_count * sizeof(uint32_t));
    for (uint32_t i = 0; i < qf_prop_count; i++) {
        qf_q_count[i] = qf_props[i].queueCount;
        printf("  index: %x count: %x\n", i, qf_props[i].queueCount);
    }

    //
    // create logical device
    //
    VkDeviceQueueCreateInfo *dev_q_infos = malloc(qf_prop_count * sizeof(VkDeviceQueueCreateInfo));
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

    dev_info.enabledExtensionCount = ENABLED_DEV_EXT_COUNT;
    char pp_exts[ENABLED_DEV_EXT_COUNT][VK_MAX_EXTENSION_NAME_SIZE];

    strcpy_s(pp_exts[0], sizeof(pp_exts[0]), "VK_KHR_swapchain");

    printf("using extensions:\n");
    char *pp_ext_names[ENABLED_DEV_EXT_COUNT];
    for (uint32_t i = 0; i < ENABLED_DEV_EXT_COUNT; i++) {
        pp_ext_names[i] = pp_exts[i];
        printf("  %s\n", pp_ext_names[i]);
    }
    dev_info.ppEnabledExtensionNames = (const char *const *) pp_ext_names;
    VkPhysicalDeviceFeatures phy_dev_feat;
    vkGetPhysicalDeviceFeatures(vcw_phy_dev->dev, &phy_dev_feat);
    dev_info.pEnabledFeatures = &phy_dev_feat;

    vkCreateDevice(vcw_phy_dev->dev, &dev_info, NULL, &VCW_DEV->dev);
    printf("logical device created.\n");
    //
    // select best queue
    //
    uint32_t qf_graph_count = 0;
    uint32_t *qf_graph_list = malloc(qf_prop_count * sizeof(uint32_t));
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
    VCW_DEV->qf_best_idx = qf_best_idx;

    vkGetDeviceQueue(VCW_DEV->dev, VCW_DEV->qf_best_idx, 0, &VCW_DEV->q_graph);
    char single_queue = 1;
    if (qf_props[VCW_DEV->qf_best_idx].queueCount < 2) {
        vkGetDeviceQueue(VCW_DEV->dev, VCW_DEV->qf_best_idx, 0, &VCW_DEV->q_pres);
        printf("using single queue for drawing.\n");
    } else {
        single_queue = 0;
        vkGetDeviceQueue(VCW_DEV->dev, VCW_DEV->qf_best_idx, 1, &VCW_DEV->q_pres);
        printf("using double queues for drawing.\n");
    }
    VCW_DEV->single_queue = single_queue;

    return VCW_DEV;
}

static void cursor_position_callback(GLFWwindow *window, double x, double y) {
    // printf("cursor position xpos:%f ypos:%f\n", x, y);
}

static void framebuffer_resize_callback(GLFWwindow *window, int width, int height) {
    printf("framebuffer resized.\n");

    VCW_SURF->resized = 1;
}

void update_surface_info(VCW_Surface *vcw_surf, VCW_PhysicalDevice vcw_phy_dev) {
    //
    // fetch surface capabilities and window size
    //
    VkSurfaceCapabilitiesKHR caps;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(vcw_phy_dev.dev, VCW_SURF->surf, &caps);
    printf("fetched caps from surface.\n");
    int wind_w, wind_h;
    glfwGetFramebufferSize(vcw_surf->window, &wind_w, &wind_h);
    uint32_t conv_w = (uint32_t) wind_w;
    uint32_t conv_h = (uint32_t) wind_h;
    vcw_surf->window_extent = (VkExtent2D) {conv_w, conv_h};

    //
    // compare extents
    //
    char extent_suitable = 1;
    VkExtent2D actual_extent;
    actual_extent = vcw_surf->window_extent;
    if (caps.currentExtent.width != wind_w || caps.currentExtent.height != wind_h) {
        extent_suitable = 0;
        printf("actual extent size doesn't match framebuffers, resizing...\n");
        actual_extent.width = conv_w > caps.maxImageExtent.width ? caps.maxImageExtent.width : conv_w;
        actual_extent.width = conv_w < caps.minImageExtent.width ? caps.minImageExtent.width : conv_w;
        actual_extent.height = conv_h > caps.maxImageExtent.height ? caps.maxImageExtent.height : conv_h;
        actual_extent.height = conv_h < caps.minImageExtent.height ? caps.minImageExtent.height : conv_h;
    }
    vcw_surf->caps = caps;
    vcw_surf->extent_suitable = extent_suitable;
    vcw_surf->actual_extent = actual_extent;
}

VCW_Surface *create_surf(VkInstance inst, VCW_PhysicalDevice vcw_phy_dev, VCW_Device vcw_dev, VkExtent2D dim) {
    VCW_SURF = malloc(sizeof(VCW_Surface));
    //
    // create window
    //
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE); // TODO: support dynamic window size
    VCW_SURF->window = glfwCreateWindow(dim.width, dim.height, "Vk Wrapper", NULL, NULL);
    glfwMaximizeWindow(VCW_SURF->window);
    VCW_SURF->window_extent = dim;
    VCW_SURF->resized = 0;
    printf("window created.\n");
    //
    // create surface
    //
    glfwCreateWindowSurface(inst, VCW_SURF->window, NULL, &VCW_SURF->surf);
    printf("surface created.\n");
    //
    // create callbacks
    //
    glfwSetCursorPosCallback(VCW_SURF->window, cursor_position_callback);
    printf("cursor position callback created.\n");
    glfwSetFramebufferSizeCallback(VCW_SURF->window, framebuffer_resize_callback);
    printf("framebuffer resize callback created.\n");
    //
    // verify surface support
    //
    vkGetPhysicalDeviceSurfaceSupportKHR(vcw_phy_dev.dev, vcw_dev.qf_best_idx, VCW_SURF->surf, &VCW_SURF->supported);
    if (VCW_SURF->supported == VK_TRUE) {
        printf("surface supported.\n");
    } else {
        printf("warning: surface unsupported!\n");
    }
    //
    // get actual extent
    //
    update_surface_info(VCW_SURF, vcw_phy_dev);
    //
    // fetch surface formats
    //
    vkGetPhysicalDeviceSurfaceFormatsKHR(vcw_phy_dev.dev, VCW_SURF->surf, &VCW_SURF->form_count, NULL);
    VCW_SURF->forms = malloc(VCW_SURF->form_count * sizeof(VkSurfaceFormatKHR));
    vkGetPhysicalDeviceSurfaceFormatsKHR(vcw_phy_dev.dev, VCW_SURF->surf, &VCW_SURF->form_count, VCW_SURF->forms);
    printf("fetched %d surface formats.\n", VCW_SURF->form_count);
    for (uint32_t i = 0; i < VCW_SURF->form_count; i++) {
        printf("format: %d\tcolorspace: %d\n", VCW_SURF->forms[i].format, VCW_SURF->forms[i].colorSpace);
    }
    //
    // fetch surface present mode
    // TODO: add dynamic present mode support
    //
    uint32_t pres_mode_count;
    vkGetPhysicalDeviceSurfacePresentModesKHR(vcw_phy_dev.dev, VCW_SURF->surf, &pres_mode_count, NULL);
    VkPresentModeKHR *pres_modes = malloc(pres_mode_count * sizeof(VkPresentModeKHR));
    vkGetPhysicalDeviceSurfacePresentModesKHR(vcw_phy_dev.dev, VCW_SURF->surf, &pres_mode_count, pres_modes);
    printf("fetched %d present modes.\n", pres_mode_count);
    char mailbox_mode_supported = 0;
    for (uint32_t i = 0; i < pres_mode_count; i++) {
        printf("present mode: %d\n", pres_modes[i]);
        if (pres_modes[i] == VK_PRESENT_MODE_MAILBOX_KHR) {
            printf("mailbox present mode supported.\n");
            mailbox_mode_supported = 1;
        }
    }
    VCW_SURF->mailbox_mode_supported = mailbox_mode_supported;

    return VCW_SURF;
}

VCW_Swapchain *create_swap(VCW_Device vcw_dev, VCW_Surface vcw_surf, VkSwapchainKHR old) {
    VCW_SWAP = malloc(sizeof(VCW_Swapchain));
    //
    // create swapchain
    //
    VkSwapchainCreateInfoKHR swap_info;
    swap_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swap_info.pNext = NULL;
    swap_info.flags = 0;
    swap_info.surface = vcw_surf.surf;
    swap_info.minImageCount = vcw_surf.caps.minImageCount + 1;
    swap_info.imageFormat = vcw_surf.forms[0].format;
    swap_info.imageColorSpace = vcw_surf.forms[0].colorSpace;
    VCW_SWAP->extent = vcw_surf.extent_suitable ? vcw_surf.caps.currentExtent : vcw_surf.actual_extent;
    swap_info.imageExtent = VCW_SWAP->extent;
    swap_info.imageArrayLayers = 1;
    swap_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    swap_info.imageSharingMode = vcw_dev.single_queue ? VK_SHARING_MODE_EXCLUSIVE : VK_SHARING_MODE_CONCURRENT;
    swap_info.queueFamilyIndexCount = vcw_dev.single_queue ? 0 : 2;
    uint32_t qf_indices[2] = {0, 1};
    swap_info.pQueueFamilyIndices = vcw_dev.single_queue ? NULL : qf_indices;
    swap_info.preTransform = vcw_surf.caps.currentTransform;
    swap_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swap_info.presentMode = vcw_surf.mailbox_mode_supported ? VK_PRESENT_MODE_MAILBOX_KHR : VK_PRESENT_MODE_FIFO_KHR;
    swap_info.clipped = VK_TRUE;
    swap_info.oldSwapchain = old;

    vkCreateSwapchainKHR(vcw_dev.dev, &swap_info, NULL, &VCW_SWAP->swap);
    printf("swapchain created.\n");
    //
    // fetch image from swapchain
    //
    vkGetSwapchainImagesKHR(vcw_dev.dev, VCW_SWAP->swap, &VCW_SWAP->img_count, NULL);
    VCW_SWAP->imgs = malloc(VCW_SWAP->img_count * sizeof(VkImage));
    vkGetSwapchainImagesKHR(vcw_dev.dev, VCW_SWAP->swap, &VCW_SWAP->img_count, VCW_SWAP->imgs);
    printf("%d images fetched from swapchain.\n", VCW_SWAP->img_count);
    //
    // create image view
    //
    VCW_SWAP->img_views = malloc(VCW_SWAP->img_count * sizeof(VkImageView));
    VkImageViewCreateInfo *img_view_infos = malloc(VCW_SWAP->img_count * sizeof(VkImageViewCreateInfo));

    VkImageSubresourceRange subres;
    subres = DEFAULT_SUBRESOURCE_RANGE;
    subres.layerCount = swap_info.imageArrayLayers;

    for (uint32_t i = 0; i < VCW_SWAP->img_count; i++) {
        img_view_infos[i].sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        img_view_infos[i].pNext = NULL;
        img_view_infos[i].flags = 0;
        img_view_infos[i].image = VCW_SWAP->imgs[i];
        img_view_infos[i].viewType = VK_IMAGE_VIEW_TYPE_2D;
        img_view_infos[i].format = vcw_surf.forms[0].format;
        img_view_infos[i].components = DEFAULT_COMPONENT_MAPPING;
        img_view_infos[i].subresourceRange = subres;
        vkCreateImageView(vcw_dev.dev, &img_view_infos[i], NULL, &VCW_SWAP->img_views[i]);
        printf("image view %d created.\n", i);
    }

    return VCW_SWAP;
}

VCW_CommandPool create_cmd_pool(VCW_Device vcw_dev, VCW_Swapchain vcw_swap) {
    VCW_CommandPool vcw_cmd;
    //
    // create command pool
    //
    VkCommandPoolCreateInfo cmd_pool_info;
    cmd_pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    cmd_pool_info.pNext = NULL;
    cmd_pool_info.flags = 0;
    cmd_pool_info.queueFamilyIndex = vcw_dev.qf_best_idx;

    vkCreateCommandPool(vcw_dev.dev, &cmd_pool_info, NULL, &vcw_cmd.cmd_pool);
    printf("command pool created.\n");
    //
    // allocate command buffers
    //
    VkCommandBufferAllocateInfo cmd_alloc_info;
    cmd_alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    cmd_alloc_info.pNext = NULL;
    cmd_alloc_info.commandPool = vcw_cmd.cmd_pool;
    cmd_alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    vcw_cmd.cmd_buf_count = vcw_swap.img_count;
    cmd_alloc_info.commandBufferCount = vcw_cmd.cmd_buf_count;

    vcw_cmd.cmd_bufs = malloc(vcw_cmd.cmd_buf_count * sizeof(VkCommandBuffer));
    vkAllocateCommandBuffers(vcw_dev.dev, &cmd_alloc_info, vcw_cmd.cmd_bufs);
    printf("command buffers allocated.\n");

    return vcw_cmd;
}

void clean_up_swap(VCW_Device vcw_dev, VCW_Swapchain swap) {
    for (uint32_t i = 0; i < swap.img_count; i++) {
        vkDestroyImageView(vcw_dev.dev, swap.img_views[i], NULL);
        printf("image view %d destroyed.\n", i);
    }
    free(swap.img_views);
    vkDestroySwapchainKHR(vcw_dev.dev, swap.swap, NULL);
    printf("swapchain destroyed.\n");
}

void clean_up_cmd_pool(VCW_Device vcw_dev, VCW_CommandPool vcw_cmd) {
    vkFreeCommandBuffers(vcw_dev.dev, vcw_cmd.cmd_pool, vcw_cmd.cmd_buf_count, vcw_cmd.cmd_bufs);
    free(vcw_cmd.cmd_bufs);
    printf("command buffers freed.\n");
    vkDestroyCommandPool(vcw_dev.dev, vcw_cmd.cmd_pool, NULL);
    printf("command pool destroyed.\n");
}


void destroy_vk_core(VkInstance inst, VCW_Device vcw_dev, VCW_Swapchain vcw_swap, VCW_Surface vcw_surf,
                     VCW_CommandPool vcw_cmd) {
    //
    // clean up cmd pool
    //
    clean_up_cmd_pool(vcw_dev, vcw_cmd);
    //
    // clean up swapchain
    //
    clean_up_swap(vcw_dev, vcw_swap);
    //
    // destroy surface and window
    //
    vkDestroySurfaceKHR(inst, vcw_surf.surf, NULL);
    printf("surface destroyed.\n");
    glfwDestroyWindow(vcw_surf.window);
    printf("window destroyed.\n");
    //
    // destroy device
    //
    vkDestroyDevice(vcw_dev.dev, NULL);
    printf("logical device destroyed.\n");
    //
    // destroy instance
    //
    vkDestroyInstance(inst, NULL);
    printf("instance destroyed.\n");

    glfwTerminate();
}
