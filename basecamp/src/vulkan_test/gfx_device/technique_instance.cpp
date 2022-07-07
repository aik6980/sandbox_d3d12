#include "technique_instance.h"

#include "device.h"
#include "resource_manager.h"
#include "technique.h"

void VKN::Technique_instance::set_constant(const std::string name, const void* data, size_t size)
{
    auto&& resource_manager = m_tech.m_gfx_device.m_resource_manager;
    auto&& frame_resource   = m_tech.m_gfx_device.curr_frame_resource();

    // allocate buffer
    auto&& buffer = std::make_shared<Buffer>(resource_manager->create_constant_buffer(data, size));
    frame_resource.m_buffers.emplace_back(buffer);
    m_constant_buffer_map.insert({name, buffer});
}
