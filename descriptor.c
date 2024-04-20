#include "descripor.h"

VCW_DescriptorPool create_vcw_desc() {
    VCW_DescriptorPool vcw_desc;
    vcw_desc.layouts = malloc(sizeof(VkDescriptorSetLayout));
    vcw_desc.set_count = 0;
    vcw_desc.pool = 0;
    vcw_desc.sets = malloc(sizeof(VkDescriptorSet));

    return vcw_desc;
}

void add_desc_set_layout(VCW_Device vcw_dev, VCW_DescriptorPool *vcw_desc,
                         VkDescriptorType type, VkShaderStageFlags stage,
                         uint32_t binding_count) {
    //
    // create descriptor set bindings
    //
    VkDescriptorSetLayoutBinding *set_bindings = malloc(sizeof(VkDescriptorSetLayoutBinding) * binding_count);
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
    vkCreateDescriptorSetLayout(vcw_dev.dev, &set_layout_info, NULL, &set_layout);
    printf("created descriptor set layout.\n");

    vcw_desc->set_count++;
    vcw_desc->layouts = realloc(vcw_desc->layouts, vcw_desc->set_count * sizeof(VkDescriptorSetLayout));
    vcw_desc->layouts[vcw_desc->set_count - 1] = set_layout;
    printf("add descriptor set layout to descriptor group.\n");
}

void init_desc_pool(VCW_Device vcw_dev, VCW_DescriptorPool *vcw_desc,
                    VkDescriptorPoolSize *sizes, uint32_t size_count) {
    //
    // create descriptor pool
    //
    VkDescriptorPoolCreateInfo pool_info;
    pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    pool_info.poolSizeCount = size_count;
    pool_info.pPoolSizes = sizes;
    pool_info.maxSets = vcw_desc->set_count;
    pool_info.pNext = NULL;
    pool_info.flags = 0;

    vkCreateDescriptorPool(vcw_dev.dev, &pool_info, NULL, &vcw_desc->pool);
    //
    // allocate descriptor sets on device
    //
    VkDescriptorSetAllocateInfo alloc_info;
    alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    alloc_info.descriptorPool = vcw_desc->pool;
    alloc_info.descriptorSetCount = vcw_desc->set_count;
    alloc_info.pSetLayouts = vcw_desc->layouts;
    alloc_info.pNext = NULL;

    vcw_desc->sets = malloc(sizeof(VkDescriptorSet) * vcw_desc->set_count);
    vkAllocateDescriptorSets(vcw_dev.dev, &alloc_info, vcw_desc->sets);
}

void write_buffer_desc(VCW_Device vcw_dev, VCW_DescriptorPool *vcw_desc,
                       VCW_Buffer *g_buf, uint32_t set, uint32_t binding,
                       VkDescriptorType desc_type) {
    VkDescriptorBufferInfo desc_buf_info;
    desc_buf_info.buffer = g_buf->buf;
    desc_buf_info.offset = 0;
    desc_buf_info.range = g_buf->size;

    VkWriteDescriptorSet desc_write;
    desc_write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    desc_write.dstSet = vcw_desc->sets[set];
    desc_write.dstBinding = binding;
    desc_write.dstArrayElement = 0;
    desc_write.descriptorType = desc_type;
    desc_write.descriptorCount = 1;
    desc_write.pBufferInfo = &desc_buf_info;
    desc_write.pNext = NULL;

    vkUpdateDescriptorSets(vcw_dev.dev, 1, &desc_write, 0, NULL);
}