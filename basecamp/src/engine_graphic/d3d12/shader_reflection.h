#pragma once

#include "input_layout_desc.h"

namespace D3D12 {
struct CBUFFER_VARIABLE_INFO {
    string                     m_name;
    D3D12_SHADER_VARIABLE_DESC m_desc;
};

struct CBUFFER_INFO {
    string                   m_name;
    D3D12_SHADER_BUFFER_DESC m_desc;

    unordered_map<string, CBUFFER_VARIABLE_INFO> m_variable_infos;
};

enum Shader_input_type
{
    Sit_cbuffer,
    Sit_srv,
    Sit_uav,
    Sit_sampler,
    Sit_count,
};

struct Shader_reflection_info {

    void generate_bound_resource_desc(const D3D12_SHADER_INPUT_BIND_DESC& desc);

    // bound resource desc
    array<vector<D3D12_SHADER_INPUT_BIND_DESC>, Sit_count> m_shader_input_descs;

    // vector<D3D12_SHADER_INPUT_BIND_DESC> m_cbuffer_desc;
    // vector<D3D12_SHADER_INPUT_BIND_DESC> m_srv_desc;
    // vector<D3D12_SHADER_INPUT_BIND_DESC> m_uav_desc;
    // vector<D3D12_SHADER_INPUT_BIND_DESC> m_sampler_desc;
    // vector<D3D12_SHADER_INPUT_BIND_DESC> m_rtaccel_structure_desc;

    // resource info
    unordered_map<string, CBUFFER_INFO> m_cbuffer_infos;
};

class ShaderReflection {
  public:
    static DXGI_FORMAT              map_to_dxgi_format(D3D_REGISTER_COMPONENT_TYPE component_type, BYTE mask);
    static D3D12_INPUT_ELEMENT_DESC map_to_d3d_input_element_desc(const D3D12_SIGNATURE_PARAMETER_DESC& src, bool is_instance_data);

    void get_reflection(ComPtr<ID3DBlob> buffer);

    const INPUT_LAYOUT_DESC& get_input_layout_desc() { return m_input_layout_desc; }

    const vector<D3D12_SHADER_INPUT_BIND_DESC>& cbuffer_binding_desc() const { return m_infos.m_shader_input_descs[Sit_cbuffer]; };
    const vector<D3D12_SHADER_INPUT_BIND_DESC>& srv_binding_desc() const { return m_infos.m_shader_input_descs[Sit_srv]; };
    const vector<D3D12_SHADER_INPUT_BIND_DESC>& uav_binding_desc() const { return m_infos.m_shader_input_descs[Sit_uav]; };
    const vector<D3D12_SHADER_INPUT_BIND_DESC>& sampler_binding_desc() const { return m_infos.m_shader_input_descs[Sit_sampler]; };

    const CBUFFER_INFO* get_cbuffer_desc(const string& name)
    {
        auto&& found = m_infos.m_cbuffer_infos.find(name);
        if (found != m_infos.m_cbuffer_infos.end()) {
            return &found->second;
        }

        return nullptr;
    }

  private:
    void generate_input_layout_desc();
    void generate_bound_resource_desc();
    void generate_cbuffer_desc();
    void generate_output_parameter_desc();

    ComPtr<ID3D12ShaderReflection> m_shader_reflection;
    D3D12_SHADER_DESC              m_shader_desc;

    // input layout desc
    INPUT_LAYOUT_DESC m_input_layout_desc;

    Shader_reflection_info m_infos;
};

class Lib_ray_reflection {
  public:
    Lib_ray_reflection();

    void get_reflection(ComPtr<ID3DBlob> buffer);

    const string& raygen_entry() { return m_raygen_entry; }
    const string& miss_entry() { return m_miss_entry; }
    const string& closethit_entry() { return m_closethit_entry; }

    const string& hitgroup() { return m_hitgroup; }

  private:
    void generate_bound_resource_desc(const shared_ptr<ID3D12FunctionReflection> func_reflection, uint32_t func_idx);

    ComPtr<ID3D12LibraryReflection> m_lib_reflection;
    D3D12_LIBRARY_DESC              m_lib_desc;

    vector<shared_ptr<ID3D12FunctionReflection>> m_function_reflections;
    vector<Shader_reflection_info>               m_infos;

    string m_raygen_entry;
    string m_miss_entry;
    string m_closethit_entry;

    string m_hitgroup;
};

}; // namespace D3D12
