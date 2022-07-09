#include "technique_instance.h"

#include "device.h"
#include "resource_manager.h"
#include "shader.h"
#include "technique.h"

namespace VKN {

    void Technique_instance::set_constant(const std::string name, const void* data, size_t size)
    {
        auto&& resource_manager = m_tech.m_gfx_device.m_resource_manager;
        auto&& frame_resource   = m_tech.m_gfx_device.curr_frame_resource();

        // allocate buffer
        auto&& buffer = std::make_shared<Buffer>(resource_manager->create_constant_buffer(data, size));
        frame_resource.m_buffers.emplace_back(buffer);
        m_constant_buffer_map.insert({name, buffer});
    }

    void Technique_instance::set_descriptor_set_parameters()
    {
        auto&& device             = m_tech.m_gfx_device.m_device;
        auto&& descriptor_layouts = m_tech.m_descriptorset_layouts;
        auto&& descriptor_info    = m_tech.m_descriptorset_infos;
        auto&& descriptor_pool    = m_tech.m_gfx_device.curr_frame_resource().m_descriptor_pool;
        auto&& command_buffer     = m_tech.m_gfx_device.curr_frame_resource().m_command_buffer;

        for (uint32_t i = 0; i < descriptor_layouts.size(); ++i) {
            // [to improve] we should support more than one binding per set
            auto&& name = descriptor_info[i]->binding_names[0];

            auto&& result = m_constant_buffer_map.find(name);
            if (result != m_constant_buffer_map.end()) {

                auto&& descriptor_set = descriptor_pool.create_descriptor_set(descriptor_layouts[i]);

                vk::DescriptorBufferInfo buffer_info{};
                buffer_info.buffer = result->second.lock()->m_buffer;
                buffer_info.offset = 0;
                buffer_info.range  = VK_WHOLE_SIZE;

                vk::WriteDescriptorSet descriptor_write{};
                descriptor_write.dstSet          = descriptor_set;
                descriptor_write.dstBinding      = descriptor_info[i]->bindings[0].binding;
                descriptor_write.dstArrayElement = 0;
                descriptor_write.descriptorType  = descriptor_info[i]->bindings[0].descriptorType;
                descriptor_write.descriptorCount = 1;
                descriptor_write.pBufferInfo     = &buffer_info;

                device.updateDescriptorSets(descriptor_write, {});

                command_buffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, m_tech.m_pipeline_layout, 0, descriptor_set, {});
            }
        }
    }

} // namespace VKN
