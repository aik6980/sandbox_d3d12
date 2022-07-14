#include "frame_resource.h"

#include "device.h"
#include "resource_manager.h"

namespace VKN {
    void Frame_resource::destroy_resources()
    {
        auto&& resource_manager = m_gfx_device.m_resource_manager;

        for (auto&& buffer : m_buffers) {
            resource_manager->destroy_buffer(*buffer);
        }
        m_buffers.clear();

        m_descriptor_pool.destroy_resources();
    }

    void Frame_resource::begin_frame()
    {
        auto&& resource_manager = m_gfx_device.m_resource_manager;

        for (auto&& buffer : m_buffers) {
            resource_manager->destroy_buffer(*buffer);
        }
        m_buffers.clear();

        m_descriptor_pool.reset();

        m_command_buffer.reset();
        m_command_buffer.begin(vk::CommandBufferBeginInfo(vk::CommandBufferUsageFlags()));
        m_command_buffer_opened = true;
    }

    void Frame_resource::end_frame()
    {
        if (m_command_buffer_opened) {
            m_command_buffer.end();
        }

        m_command_buffer_opened = false;
    }
} // namespace VKN
