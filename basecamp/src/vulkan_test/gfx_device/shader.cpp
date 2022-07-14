#include "shader.h"

#include "device.h"

namespace VKN {

    void Shader::destroy()
    {
        auto&& device = m_gfx_device.m_device;

        spvReflectDestroyShaderModule(&m_reflection_module);

        device.destroyShaderModule(m_shader_module);
    }

    void Shader::create_shader(const std::string& filename)
    {
        if (filename == "") {
            return;
        }

        // read from file ----------
        const auto    filename_relative = std::string("shader_spirv/" + filename + ".obj");
        std::ifstream file(filename_relative, std::ios::ate | std::ios::binary);
        if (!file.is_open()) {
            throw std::runtime_error("failed to open file!");
        }

        size_t file_size = (size_t)file.tellg();
        m_bin_data.resize(file_size);

        file.seekg(0);
        file.read(m_bin_data.data(), file_size);

        file.close();
        // ----------

        // create shader module -----------
        auto&& device = m_gfx_device.m_device;

        auto&&                     shader_code = m_bin_data;
        vk::ShaderModuleCreateInfo createinfo(vk::ShaderModuleCreateFlags(), (std::vector<uint32_t>&)shader_code);
        m_shader_module = device.createShaderModule(createinfo);
        // ----------

        // create shader reflection
        create_shader_reflection();
    }

    void Shader::create_shader_reflection()
    {
        SpvReflectResult result = spvReflectCreateShaderModule(m_bin_data.size(), m_bin_data.data(), &m_reflection_module);
        assert(result == SPV_REFLECT_RESULT_SUCCESS);

        create_vertex_input();
        create_descriptorset_layoutdata();
    }

    void Shader::create_vertex_input()
    {
        uint32_t count  = 0;
        auto&&   result = spvReflectEnumerateInputVariables(&m_reflection_module, &count, NULL);
        assert(result == SPV_REFLECT_RESULT_SUCCESS);

        std::vector<SpvReflectInterfaceVariable*> input_vars(count);
        result = spvReflectEnumerateInputVariables(&m_reflection_module, &count, input_vars.data());
        assert(result == SPV_REFLECT_RESULT_SUCCESS);

        // generate vertexinput
        // Simplifying assumptions:
        // - All vertex input attributes are sourced from a single vertex buffer,
        //   bound to VB slot 0.
        // - Each vertex's attribute are laid out in ascending order by location.
        // - The format of each attribute matches its usage in the shader;
        //   float4 -> VK_FORMAT_R32G32B32A32_FLOAT, etc. No attribute compression is applied.
        // - All attributes are provided per-vertex, not per-instance.

        // stride will be calculated later
        m_vertex_input_binding_description = vk::VertexInputBindingDescription(0, 0, vk::VertexInputRate::eVertex);

        auto&& attribute_descriptions = m_vertex_input_attribute_descriptions;
        attribute_descriptions.reserve(input_vars.size());
        for (size_t i_var = 0; i_var < input_vars.size(); ++i_var) {
            const SpvReflectInterfaceVariable& refl_var = *(input_vars[i_var]);
            // ignore built-in variables
            if (refl_var.decoration_flags & SPV_REFLECT_DECORATION_BUILT_IN) {
                continue;
            }
            vk::VertexInputAttributeDescription attr_desc{};
            attr_desc.location = refl_var.location;
            attr_desc.binding  = m_vertex_input_binding_description.binding;
            attr_desc.format   = static_cast<vk::Format>(refl_var.format);
            attr_desc.offset   = 0; // final offset computed below after sorting.
            attribute_descriptions.push_back(attr_desc);
        }
        // Sort attributes by location
        std::sort(std::begin(attribute_descriptions), std::end(attribute_descriptions), [](const auto& a, const auto& b) { return a.location < b.location; });
        // Compute final offsets of each attribute, and total vertex stride.
        for (auto& attribute : attribute_descriptions) {
            uint32_t format_size = Device::format_size(attribute.format);
            attribute.offset     = m_vertex_input_binding_description.stride;
            m_vertex_input_binding_description.stride += format_size;
        }
    }

    void Shader::create_descriptorset_layoutdata()
    {
        // usign reflection
        auto&& module = m_reflection_module;

        uint32_t count  = 0;
        auto&&   result = spvReflectEnumerateDescriptorSets(&module, &count, NULL);
        assert(result == SPV_REFLECT_RESULT_SUCCESS);

        std::vector<SpvReflectDescriptorSet*> sets(count);
        result = spvReflectEnumerateDescriptorSets(&module, &count, sets.data());
        assert(result == SPV_REFLECT_RESULT_SUCCESS);

        // Demonstrates how to generate all necessary data structures to create a
        // VkDescriptorSetLayout for each descriptor set in this shader.
        auto&& set_layouts = m_descriptorset_layoutdata;
        set_layouts.resize(sets.size());
        for (size_t i_set = 0; i_set < sets.size(); ++i_set) {
            const SpvReflectDescriptorSet& refl_set = *(sets[i_set]);
            Descriptorset_layoutdata&      layout   = set_layouts[i_set];

            layout.bindings.resize(refl_set.binding_count);
            layout.binding_names.resize(refl_set.binding_count);

            for (uint32_t i_binding = 0; i_binding < refl_set.binding_count; ++i_binding) {
                const SpvReflectDescriptorBinding& refl_binding = *(refl_set.bindings[i_binding]);

                layout.binding_names[i_binding]                = refl_binding.name;
                vk::DescriptorSetLayoutBinding& layout_binding = layout.bindings[i_binding];
                layout_binding.binding                         = refl_binding.binding;
                layout_binding.descriptorType                  = static_cast<vk::DescriptorType>(refl_binding.descriptor_type);
                layout_binding.descriptorCount                 = 1;
                for (uint32_t i_dim = 0; i_dim < refl_binding.array.dims_count; ++i_dim) {
                    layout_binding.descriptorCount *= refl_binding.array.dims[i_dim];
                }
                layout_binding.stageFlags = static_cast<vk::ShaderStageFlagBits>(module.shader_stage);
            }
            layout.set_number               = refl_set.set;
            layout.create_info.bindingCount = refl_set.binding_count;
            layout.create_info.pBindings    = layout.bindings.data();
        }
    }

} // namespace VKN
