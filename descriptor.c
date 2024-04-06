#include "descripor.h"

void add_desc_set_layout(struct DescriptorGroup *g_desc, VkDevice dev,
                         VkDescriptorType type, VkShaderStageFlags stage,
                         uint32_t binding_count) {
    //
    // create descriptor set bindings
    //
    VkDescriptorSetLayoutBindingFlagsCreateInfo info;

    VkDescriptorSetLayoutBinding set_bindings[binding_count];
    for (uint32_t i = 0; i < binding_count; i++) {
        set_bindings[i].binding = i;
        set_bindings[i].descriptorCount = 1;
        set_bindings[i].descriptorType = type;
        set_bindings[i].pImmutableSamplers = NULL;
        set_bindings[i].stageFlags = stage;
    }
    printf("created descriptor bindings.\n");
    //
    // create descriptor set layout
    //
    VkDescriptorSetLayoutCreateInfo set_layout_info;
    set_layout_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    set_layout_info.bindingCount = binding_count;
    set_layout_info.pBindings = set_bindings;
    set_layout_info.pNext = NULL;
    set_layout_info.flags = 0;

    VkDescriptorSetLayout set_layout;
    vkCreateDescriptorSetLayout(dev, &set_layout_info, NULL, &set_layout);
    printf("created descriptor set layout.\n");

    g_desc->set_count++;
    g_desc->layouts = realloc(g_desc->layouts, g_desc->set_count * sizeof(VkDescriptorSetLayout));
    g_desc->layouts[g_desc->set_count - 1] = set_layout;
    printf("add descriptor set layout to descriptor group.\n");
}

void init_desc_pool(struct DescriptorGroup *g_desc, VkDevice dev,
                    VkDescriptorPoolSize *sizes, uint32_t size_count) {
    //
    // create descriptor pool
    //
    VkDescriptorPoolCreateInfo pool_info;
    pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    pool_info.poolSizeCount = size_count;
    pool_info.pPoolSizes = sizes;
    pool_info.maxSets = g_desc->set_count;

    VkDescriptorPool pool;
    vkCreateDescriptorPool(dev, &pool_info, NULL, &pool);
    g_desc->pool = pool;
    //
    // allocate descriptor sets on device
    //
    VkDescriptorSetAllocateInfo alloc_info;
    alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    alloc_info.descriptorPool = pool;
    alloc_info.descriptorSetCount = g_desc->set_count;
    alloc_info.pSetLayouts = g_desc->layouts;

    g_desc->sets = malloc(sizeof(VkDescriptorSet) * g_desc->set_count);
    vkAllocateDescriptorSets(dev, &alloc_info, g_desc->sets);
}

void write_buffer_desc(struct DescriptorGroup *g_desc, VkDevice dev,
                       struct BufferSet *g_buf, uint32_t set, uint32_t binding,
                       VkDescriptorType desc_type) {
    VkDescriptorBufferInfo desc_buf_info;
    desc_buf_info.buffer = g_buf->buf;
    desc_buf_info.offset = 0;
    desc_buf_info.range = g_buf->size;

    VkWriteDescriptorSet desc_write;
    desc_write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    desc_write.dstSet = g_desc->sets[set];
    desc_write.dstBinding = binding;
    desc_write.dstArrayElement = 0;
    desc_write.descriptorType = desc_type;
    desc_write.descriptorCount = 1;
    desc_write.pBufferInfo = &desc_buf_info;

    vkUpdateDescriptorSets(dev, 1, &desc_write, 0, NULL);
}