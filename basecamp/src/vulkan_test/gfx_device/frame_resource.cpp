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
    }
} // namespace VKN
