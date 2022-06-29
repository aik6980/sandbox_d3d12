#pragma once

namespace VKN {

class Device;

class Shader {
  public:
    friend class Device;

    Shader(Device& device) : m_gfx_device(device) {}

    void destroy();

    void create_shader(const std::string& filename);
    void create_shader_reflection();

  private:
    Device& m_gfx_device;

    // File content
    std::vector<char> m_bin_data;

    // Shader resources
    vk::ShaderModule m_shader_module;

    vk::ShaderModule m_vertex_shader;
    vk::ShaderModule m_pixel_shader;
    // Pipeline layout
    vk::PipelineLayout m_pipeline_layout;

    // Reflection
    SpvReflectShaderModule m_reflection;

    // Reflection data
    vk::VertexInputBindingDescription                m_vertex_input_binding_description; // this will be an array with instance_data
    std::vector<vk::VertexInputAttributeDescription> m_vertex_input_attribute_descriptions;
};
} // namespace VKN
