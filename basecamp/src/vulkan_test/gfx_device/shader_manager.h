#pragma once

#include "shader.h"
#include "technique.h"

namespace VKN {
class Shader_manager {
  public:
    Shader_manager(Device& gfx_device)
        : m_gfx_device(gfx_device)
        , m_vertex_shader(gfx_device)
        , m_pixel_shader(gfx_device)
        , m_vertex_shader_2(gfx_device)
        , m_technique(gfx_device)
    {
    }

    void destroy_resources();

    Shader m_vertex_shader;
    Shader m_vertex_shader_2;
    Shader m_pixel_shader;

    Technique m_technique;

  private:
    Device& m_gfx_device;

    std::unordered_map<std::string, std::unique_ptr<Shader>> m_shader_list;
};
} // namespace VKN
