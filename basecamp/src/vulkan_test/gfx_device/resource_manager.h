#pragma once

namespace VKN {

class Device;

class Resource_manager {
  public:
    void create_vertex_buffer(uint32_t byte_size);

  private:
    Device& m_gfx_device;
};

} // namespace VKN
