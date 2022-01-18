#include "render_technique_instance.h"

#include "device.h"
#include "resource_manager.h"
#include "shader.h"
#include "shader_manager.h"

namespace D3D12 {

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

Compute_pipeline_state_desc Technique::get_compute_pipeline_state_desc()
{
    Compute_pipeline_state_desc pso_desc = {};

    auto&& cs = m_shader_mgr.get_shader(m_cs);
    pso_desc.set_shader(cs->m_buffer.Get());

    pso_desc.pRootSignature = m_root_signature.Get();

    return pso_desc;
}

void Technique::prepare_cbuffer_bindings()
{
    build_unique_cbuffer_info(m_vs);
    build_unique_cbuffer_info(m_ps);
    build_unique_cbuffer_info(m_cs);
}

void Technique::build_unique_cbuffer_info(const string& shader_name)
{
    auto&& shader = m_shader_mgr.get_shader(shader_name);
    if (!shader) {
        return;
    }

    auto&& shader_info = shader->m_reflection;

    auto&& cbuffer_bindings = shader_info->get_infos().cbuffer_binding_desc();
    for (auto&& cbuffer_binding : cbuffer_bindings) {
        string name         = cbuffer_binding.Name;
        auto&& cbuffer_desc = shader_info->get_infos().get_cbuffer_desc(name);
        if (cbuffer_desc) {
            // only add if it is not in the list
            auto&& found = std::find_if(
                m_cbuffer_infos.begin(), m_cbuffer_infos.end(), [&name](const Cbuffer_info* a) { return strcmp(a->m_desc.Name, name.c_str()) == 0; });
            if (found == m_cbuffer_infos.end()) {
                m_cbuffer_infos.emplace_back(cbuffer_desc);
            }
        }
    }
}

void Technique_instance::init(const string& technique_name)
{
    m_technique_handle = m_shader_mgr.get_render_technique(technique_name);
    if (auto&& technique = m_technique_handle.lock()) {
        init_cbuffer();
    }
}

void Technique_instance::set_cbv(const string& cbuffer_name, const string& var_name, void* data, uint32_t data_size)
{
    auto&& found_cbuffer_data = m_cbuffer.find(cbuffer_name);
    if (found_cbuffer_data != m_cbuffer.end()) {
        // validate variable info
        auto&& var_info = get_cbuffer_var_info(cbuffer_name, var_name);
        if (var_info) {
            if (var_info->Size == data_size) {
                // auto&& mapped_buffer_data = m_device.get_mapped_data(*found_cbuffer_data->second);
                auto&& mapped_buffer_data = get<void*>(found_cbuffer_data->second);
                auto&& dest_data          = (char*)mapped_buffer_data + var_info->StartOffset;

                memcpy(dest_data, data, data_size);
            }
            else {
                throw;
            }
        }
    }
}

void Technique_instance::set_srv(const string& var_name, weak_ptr<Buffer> buffer)
{
    m_srv[var_name] = buffer;
}

void Technique_instance::set_uav(const string& var_name, weak_ptr<Buffer> buffer)
{
    m_uav[var_name] = buffer;
}

void Technique_instance::set_sampler(const string& var_name, weak_ptr<Sampler> resource)
{
    m_samplers[var_name] = resource;
}

void Technique_instance::set_root_signature_parameters(ID3D12GraphicsCommandList& command_list)
{
    auto&& technique = m_technique_handle.lock();
    if (technique) {
        if (!technique->m_vs.empty()) {
            set_raster_root_signature_parameters(command_list);
        }
        else if (!technique->m_cs.empty()) {
            set_compute_root_signature_parameters(command_list);
        }
        else {
            throw;
        }
    }
}

void Technique_instance::set_raster_root_signature_parameters(ID3D12GraphicsCommandList& command_list)
{
    auto&& resource_mgr = m_device.resource_manager();

    auto&& technique = m_technique_handle.lock();
    if (technique) {

        command_list.SetGraphicsRootSignature(technique->m_root_signature.Get());

        for (uint32_t i = 0; i < technique->m_descriptor_ranges.size(); ++i) {
            auto&& name = technique->m_descriptor_table_names[i];

            auto&& found_cbuffer_data = m_cbuffer.find(name);
            if (found_cbuffer_data != m_cbuffer.end()) {
                // auto&& buffer = m_device.get_buffer(*found_cbuffer_data->second).lock();
                auto&& buffer = get<weak_ptr<Buffer>>(found_cbuffer_data->second).lock();
                if (buffer) {
                    auto&& gpu_descriptor_handle = resource_mgr.create_cbv(*buffer);
                    command_list.SetGraphicsRootDescriptorTable(i, gpu_descriptor_handle);
                }
            }

            auto&& found_srv_data = m_srv.find(name);
            if (found_srv_data != m_srv.end()) {
                auto&& buffer = found_srv_data->second.lock();
                if (buffer) {
                    auto&& gpu_descriptor_handle = resource_mgr.create_srv(*buffer);
                    command_list.SetGraphicsRootDescriptorTable(i, gpu_descriptor_handle);
                }
            }

            auto&& found_uav_data = m_uav.find(name);
            if (found_uav_data != m_uav.end()) {
                auto&& buffer = found_uav_data->second.lock();
                if (buffer) {
                    auto&& gpu_descriptor_handle = resource_mgr.create_uav(*buffer);
                    command_list.SetGraphicsRootDescriptorTable(i, gpu_descriptor_handle);
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

void Technique_instance::set_compute_root_signature_parameters(ID3D12GraphicsCommandList& command_list)
{
    auto&& resource_mgr = m_device.resource_manager();

    auto&& technique = m_technique_handle.lock();
    if (technique) {

        command_list.SetComputeRootSignature(technique->m_root_signature.Get());

        for (uint32_t i = 0; i < technique->m_descriptor_ranges.size(); ++i) {
            auto&& name = technique->m_descriptor_table_names[i];

            auto&& found_cbuffer_data = m_cbuffer.find(name);
            if (found_cbuffer_data != m_cbuffer.end()) {
                auto&& buffer = get<weak_ptr<Buffer>>(found_cbuffer_data->second).lock();
                if (buffer) {
                    auto&& gpu_descriptor_handle = resource_mgr.create_cbv(*buffer);
                    command_list.SetComputeRootDescriptorTable(i, gpu_descriptor_handle);
                }
            }

            auto&& found_srv_data = m_srv.find(name);
            if (found_srv_data != m_srv.end()) {
                auto&& buffer = found_srv_data->second.lock();
                if (buffer) {
                    auto&& gpu_descriptor_handle = resource_mgr.create_srv(*buffer);
                    command_list.SetComputeRootDescriptorTable(i, gpu_descriptor_handle);
                }
            }

            auto&& found_uav_data = m_uav.find(name);
            if (found_uav_data != m_uav.end()) {
                auto&& buffer = found_uav_data->second.lock();
                if (buffer) {
                    auto&& gpu_descriptor_handle = resource_mgr.create_uav(*buffer);
                    command_list.SetComputeRootDescriptorTable(i, gpu_descriptor_handle);
                }
            }

            auto&& found_sampler_data = m_samplers.find(name);
            if (found_sampler_data != m_samplers.end()) {
                auto&& gpu_descriptor_handle = m_device.get_gpu_descriptor_handle(found_sampler_data->second);
                if (std::get<bool>(gpu_descriptor_handle)) {
                    command_list.SetComputeRootDescriptorTable(i, std::get<CD3DX12_GPU_DESCRIPTOR_HANDLE>(gpu_descriptor_handle));
                }
            }
        }
    }
}

void Technique_instance::init_cbuffer()
{
    auto&& technique = m_technique_handle.lock();
    if (technique) {
        for (auto&& info : technique->m_cbuffer_infos) {
            auto&& name         = info->m_desc.Name;
            auto&& cbuffer_data = m_device.create_cbuffer(info->m_desc.Size, name);
            m_cbuffer.insert(std::make_pair(name, cbuffer_data));
            m_cbuffer_infos.insert(std::make_pair(name, info));
        }
    }
}

const D3D12_SHADER_VARIABLE_DESC* Technique_instance::get_cbuffer_var_info(const string& cbuffer_name, const string& var_name)
{
    auto&& found_cbuffer_info = m_cbuffer_infos.find(cbuffer_name);
    if (found_cbuffer_info != m_cbuffer_infos.end()) {

        auto&& cbuffer_desc = found_cbuffer_info->second;

        auto&& found_var = cbuffer_desc->m_variable_infos.find(var_name);
        if (found_var != cbuffer_desc->m_variable_infos.end()) {
            return &found_var->second;
        }
    }

    return nullptr;
}

// void Technique_instance::init_dynamic_cbuffer(const string& shader_name)
//{
//     auto&& shader = m_shader_mgr.get_shader(shader_name);
//     if (!shader) {
//         return;
//     }
//
//     auto&& shader_info = shader->m_reflection;
//
//     auto&& cbuffer_bindings = shader_info->get_infos().cbuffer_binding_desc();
//     for (auto&& cbuffer_binding : cbuffer_bindings) {
//         string name         = cbuffer_binding.Name;
//         auto&& cbuffer_desc = shader_info->get_infos().get_cbuffer_desc(name);
//         if (cbuffer_desc) {
//             // only add if it is not in the list
//             if (m_cbuffer.find(name) == m_cbuffer.end()) {
//                 auto&& cbuffer_data = m_device.create_cbuffer(cbuffer_desc->m_desc.Size, name);
//                 m_cbuffer.insert(std::make_pair(name, cbuffer_data));
//                 m_cbuffer_infos.insert(std::make_pair(name, cbuffer_desc));
//             }
//         }
//     }
// }

} // namespace D3D12
