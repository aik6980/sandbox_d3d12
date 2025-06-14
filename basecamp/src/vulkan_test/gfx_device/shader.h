#pragma once

namespace VKN {

    class Device;

    struct Descriptorset_layoutdata {
        uint32_t set_number;
        vk::DescriptorSetLayoutCreateInfo set_create_info;
        vk::DescriptorSetLayoutBindingFlagsCreateInfo set_binding_flags_create_info;

        std::vector<vk::DescriptorSetLayoutBinding> bindings;
        std::vector<vk::DescriptorBindingFlags> binding_flags;

        std::vector<std::string> binding_names;
    };

    class Shader {
      public:
        friend class Device;
        friend class Technique;

        Shader(Device& device)
            : m_gfx_device(device)
        {
        }

        void destroy();

        void create_shader(const std::string& filename);

      private:
        void create_shader_reflection();
        void create_vertex_input();
        void create_descriptorset_layoutdata();

        Device& m_gfx_device;

        // File content
        std::vector<char> m_bin_data;

        // Shader resources
        vk::ShaderModule m_shader_module;
        SpvReflectShaderModule m_reflection_module;

        // Reflection data
        vk::VertexInputBindingDescription m_vertex_input_binding_description; // this will be an array with instance_data
        std::vector<vk::VertexInputAttributeDescription> m_vertex_input_attribute_descriptions;

        std::vector<Descriptorset_layoutdata> m_descriptorset_layoutdata;
    };
} // namespace VKN
