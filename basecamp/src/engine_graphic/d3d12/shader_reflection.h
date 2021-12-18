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

class ShaderReflection {
  public:
    static DXGI_FORMAT              map_to_dxgi_format(D3D_REGISTER_COMPONENT_TYPE component_type, BYTE mask);
    static D3D12_INPUT_ELEMENT_DESC map_to_d3d_input_element_desc(const D3D12_SIGNATURE_PARAMETER_DESC& src, bool is_instance_data);

    void get_reflection(ComPtr<ID3DBlob> buffer);
    void get_lib_reflection(ComPtr<ID3DBlob> buffer);

    const INPUT_LAYOUT_DESC& get_input_layout_desc() { return m_input_layout_desc; }

    const vector<D3D12_SHADER_INPUT_BIND_DESC>& cbuffer_binding_desc() const { return m_cbuffer_desc; };
    const vector<D3D12_SHADER_INPUT_BIND_DESC>& srv_binding_desc() const { return m_srv_desc; };
    const vector<D3D12_SHADER_INPUT_BIND_DESC>& uav_binding_desc() const { return m_uav_desc; };
    const vector<D3D12_SHADER_INPUT_BIND_DESC>& sampler_binding_desc() const { return m_sampler_desc; };

    const CBUFFER_INFO* get_cbuffer_desc(const string& name)
    {
        auto&& found = m_cbuffer_infos.find(name);
        if (found != m_cbuffer_infos.end()) {
            return &found->second;
        }

        return nullptr;
    }

  private:
    void generate_input_layout_desc();
    void generate_bound_resource_desc();
    void generate_cbuffer_desc();
    void generate_output_parameter_desc();

    ComPtr<ID3D12ShaderReflection>  m_shader_reflection;
    ComPtr<ID3D12LibraryReflection> m_lib_reflection;

    D3D12_SHADER_DESC  m_shader_desc;
    D3D12_LIBRARY_DESC m_lib_desc;

    // input layout desc
    INPUT_LAYOUT_DESC m_input_layout_desc;

    // bound resource desc
    vector<D3D12_SHADER_INPUT_BIND_DESC> m_cbuffer_desc;
    vector<D3D12_SHADER_INPUT_BIND_DESC> m_srv_desc;
    vector<D3D12_SHADER_INPUT_BIND_DESC> m_uav_desc;
    vector<D3D12_SHADER_INPUT_BIND_DESC> m_sampler_desc;

    // resource info
    unordered_map<string, CBUFFER_INFO> m_cbuffer_infos;
};
}; // namespace D3D12
