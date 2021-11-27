#include "render_technique_instance.h"

#include "device.h"
#include "shader.h"
#include "shader_manager.h"

namespace D3D12 {
const CBUFFER_VARIABLE_INFO* Technique::get_cbuffer_var_info(const string& cbuffer_name, const string& var_name)
{
    auto&& var_info = get_cbuffer_var_info(m_vs, cbuffer_name, var_name);
    if (!var_info) {
        var_info = get_cbuffer_var_info(m_ps, cbuffer_name, var_name);
    }

    return var_info;
}

const INPUT_LAYOUT_DESC* Technique::get_input_layout_desc()
{
    auto&& shader_vs = m_shader_mgr.get_shader(m_vs);
    if (shader_vs) {
        return &shader_vs->m_reflection->get_input_layout_desc();
    }

    return nullptr;
}

Graphics_pipeline_state_desc Technique::get_graphic_pipeline_state_desc()
{
    Graphics_pipeline_state_desc pso_desc = {};
    pso_desc.NumRenderTargets             = 1;
    pso_desc.RTVFormats[0]                = DXGI_FORMAT_UNKNOWN;
    pso_desc.DSVFormat                    = DXGI_FORMAT_UNKNOWN;

    auto&& vs = m_shader_mgr.get_shader(m_vs);
    pso_desc.set_vertex_shader(vs->m_buffer.Get());

    auto&& ps = m_shader_mgr.get_shader(m_ps);
    pso_desc.set_pixel_shader(ps ? ps->m_buffer.Get() : nullptr);

    auto&& input_layout = get_input_layout_desc();
    if (input_layout) {
        pso_desc.InputLayout = input_layout->as_d3d12_input_layout_desc();
    }
    pso_desc.pRootSignature = m_root_signature.Get();

    return pso_desc;
}

const CBUFFER_INFO* Technique::get_cbuffer_info(const string& shader_name, const string& cbuffer_name)
{
    auto&& shader_vs   = m_shader_mgr.get_shader(shader_name);
    auto&& shader_info = shader_vs->m_reflection;

    auto&& cbuffer_desc = shader_info->get_cbuffer_desc(cbuffer_name);
    return cbuffer_desc;
}

const CBUFFER_VARIABLE_INFO* Technique::get_cbuffer_var_info(const string& shader_name, const string& cbuffer_name, const string& var_name)
{
    auto&& cbuffer_desc = get_cbuffer_info(shader_name, cbuffer_name);
    if (cbuffer_desc) {
        auto&& found_var = cbuffer_desc->m_variable_infos.find(var_name);
        if (found_var != cbuffer_desc->m_variable_infos.end()) {
            return &found_var->second;
        }
    }

    return nullptr;
}

void D3D12::TechniqueInstance::init(const string& technique_name)
{
    m_technique_handle = m_shader_mgr.get_render_technique(technique_name);
    if (auto&& technique = m_technique_handle.lock()) {
        init_dynamic_cbuffer(technique->m_vs);
        init_dynamic_cbuffer(technique->m_ps);
    }
}

void D3D12::TechniqueInstance::set_cbv(const string& cbuffer_name, const string& var_name, void* data, uint32_t data_size)
{
    auto&& found_cbuffer_data = m_cbuffer.find(cbuffer_name);
    if (auto&& technique = m_technique_handle.lock()) {
        if (found_cbuffer_data != m_cbuffer.end()) {
            // validate variable info
            auto&& var_info = technique->get_cbuffer_var_info(cbuffer_name, var_name);
            if (var_info) {
                if (var_info->m_desc.Size == data_size) {
                    auto&& mapped_buffer_data = m_device.get_mapped_data(*found_cbuffer_data->second);
                    auto&& dest_data          = (char*)mapped_buffer_data + var_info->m_desc.StartOffset;

                    memcpy(dest_data, data, data_size);
                }
                else {
                    throw;
                }
            }
        }
    }
}

void TechniqueInstance::set_srv(const string& var_name, weak_ptr<Buffer> buffer)
{
    m_srv[var_name] = buffer;
}

void TechniqueInstance::set_sampler(const string& var_name, weak_ptr<Sampler> resource)
{
    m_samplers[var_name] = resource;
}

void TechniqueInstance::set_root_signature_parameters(ID3D12GraphicsCommandList& command_list)
{
    auto&& technique = m_technique_handle.lock();
    if (technique) {
        command_list.SetGraphicsRootSignature(technique->m_root_signature.Get());

        for (uint32_t i = 0; i < technique->m_descriptor_ranges.size(); ++i) {
            auto&& name = technique->m_descriptor_table_names[i];

            auto&& found_cbuffer_data = m_cbuffer.find(name);
            if (found_cbuffer_data != m_cbuffer.end()) {
                auto&& gpu_descriptor_handle = m_device.get_gpu_descriptor_handle(*found_cbuffer_data->second);
                if (std::get<bool>(gpu_descriptor_handle)) {
                    command_list.SetGraphicsRootDescriptorTable(i, std::get<CD3DX12_GPU_DESCRIPTOR_HANDLE>(gpu_descriptor_handle));
                }
            }

            auto&& found_srv_data = m_srv.find(name);
            if (found_srv_data != m_srv.end()) {
                auto&& gpu_descriptor_handle = m_device.get_gpu_descriptor_handle(found_srv_data->second);
                if (std::get<bool>(gpu_descriptor_handle)) {
                    command_list.SetGraphicsRootDescriptorTable(i, std::get<CD3DX12_GPU_DESCRIPTOR_HANDLE>(gpu_descriptor_handle));
                }
            }

            auto&& found_sampler_data = m_samplers.find(name);
            if (found_sampler_data != m_samplers.end()) {
                auto&& gpu_descriptor_handle = m_device.get_gpu_descriptor_handle(found_sampler_data->second);
                if (std::get<bool>(gpu_descriptor_handle)) {
                    command_list.SetGraphicsRootDescriptorTable(i, std::get<CD3DX12_GPU_DESCRIPTOR_HANDLE>(gpu_descriptor_handle));
                }
            }
        }
    }
}

void TechniqueInstance::init_dynamic_cbuffer(const string& shader_name)
{
    auto&& shader = m_shader_mgr.get_shader(shader_name);
    if (!shader) {
        return;
    }

    auto&& shader_info = shader->m_reflection;

    auto&& cbuffer_bindings = shader_info->cbuffer_binding_desc();
    for (auto&& cbuffer_binding : cbuffer_bindings) {
        string name         = cbuffer_binding.Name;
        auto&& cbuffer_desc = shader_info->get_cbuffer_desc(name);
        if (cbuffer_desc) {
            // only add if it is not in the list
            if (m_cbuffer.find(name) == m_cbuffer.end()) {
                auto&& cbuffer_data = m_device.create_dynamic_cbuffer(cbuffer_desc->m_desc.Size, name);
                m_cbuffer.insert(std::make_pair(name, cbuffer_data));
            }
        }
    }
}

} // namespace D3D12
