#include "shader_manager.h"

namespace VKN {

void Shader_manager::destroy_resources()
{
    m_technique.destroy();

    m_pixel_shader.destroy();
    m_vertex_shader_2.destroy();
    m_vertex_shader.destroy();
}

} // namespace VKN
