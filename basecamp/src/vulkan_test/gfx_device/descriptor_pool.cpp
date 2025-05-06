#include "descriptor_pool.h"

#include "device.h"

namespace VKN {

    void Descriptor_pool::create_pool()
    {
        auto&& device = m_gfx_device.m_device;

        auto&& pool_size   = vk::DescriptorPoolSize(vk::DescriptorType::eUniformBuffer, Descriptor_pool::m_max_descriptor);
        auto&& create_info = vk::DescriptorPoolCreateInfo {
            .poolSizeCount = Descriptor_pool::m_max_descriptor,
            .pPoolSizes = &pool_size
        };

        m_descriptor_pool = device.createDescriptorPool(create_info);
        m_descriptor_sets.reserve(Descriptor_pool::m_max_descriptor);
    }

    void Descriptor_pool::destroy_resources()
    {
        m_gfx_device.m_device.destroyDescriptorPool(m_descriptor_pool);
        m_descriptor_sets.clear();
    }

    void Descriptor_pool::reset()
    {
        m_gfx_device.m_device.resetDescriptorPool(m_descriptor_pool);
        m_descriptor_sets.clear();
    }

    vk::DescriptorSet& Descriptor_pool::create_descriptor_set(const vk::DescriptorSetLayout& layout)
    {
        auto&& device = m_gfx_device.m_device;

        vk::DescriptorSetAllocateInfo alloc_info{};
        alloc_info.descriptorPool     = m_descriptor_pool;
        alloc_info.descriptorSetCount = 1;
        alloc_info.pSetLayouts        = &layout;

        auto&& descriptor_sets = device.allocateDescriptorSets(alloc_info);
        return m_descriptor_sets.emplace_back(descriptor_sets[0]);
    }
} // namespace VKN
