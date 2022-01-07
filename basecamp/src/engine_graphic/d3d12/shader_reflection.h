#pragma once

#include "input_layout_desc.h"

namespace D3D12 {

struct Cbuffer_info {
    D3D12_SHADER_BUFFER_DESC                          m_desc;
    unordered_map<string, D3D12_SHADER_VARIABLE_DESC> m_variable_infos;
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

    const vector<D3D12_SHADER_INPUT_BIND_DESC>& cbuffer_binding_desc() const { return m_shader_input_descs[Sit_cbuffer]; };
    const vector<D3D12_SHADER_INPUT_BIND_DESC>& srv_binding_desc() const { return m_shader_input_descs[Sit_srv]; };
    const vector<D3D12_SHADER_INPUT_BIND_DESC>& uav_binding_desc() const { return m_shader_input_descs[Sit_uav]; };
    const vector<D3D12_SHADER_INPUT_BIND_DESC>& sampler_binding_desc() const { return m_shader_input_descs[Sit_sampler]; };

    const Cbuffer_info* get_cbuffer_desc(const string& name) const
    {
        auto&& found = m_cbuffer_infos.find(name);
        if (found != m_cbuffer_infos.end()) {
            return &found->second;
        }

        return nullptr;
    }

    // bound resource desc
    array<vector<D3D12_SHADER_INPUT_BIND_DESC>, Sit_count> m_shader_input_descs;
    // resource info
    unordered_map<string, Cbuffer_info> m_cbuffer_infos;
};

class ShaderReflection {
  public:
    static DXGI_FORMAT              map_to_dxgi_format(D3D_REGISTER_COMPONENT_TYPE component_type, BYTE mask);
    static D3D12_INPUT_ELEMENT_DESC map_to_d3d_input_element_desc(const D3D12_SIGNATURE_PARAMETER_DESC& src, bool is_instance_data);

    void get_reflection(ComPtr<ID3DBlob> buffer);

    const INPUT_LAYOUT_DESC&      get_input_layout_desc() { return m_input_layout_desc; }
    const Shader_reflection_info& get_infos() { return m_infos; }

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

struct Lib_ray_sub_shader {
    string                    m_name;
    ID3D12FunctionReflection* m_func_reflection = nullptr;
    D3D12_FUNCTION_DESC       m_func_desc;
    Shader_reflection_info    m_func_input_info;
};

class Lib_ray_reflection {
  public:
    Lib_ray_reflection();

    void get_reflection(ComPtr<ID3DBlob> buffer);

    inline static const string raygen_entry    = "raygen_entry";
    inline static const string miss_entry      = "miss_entry";
    inline static const string closethit_entry = "closethit_entry";
    inline static const string hitgroup        = "hitgroup";

    const Shader_reflection_info& get_global_input_desc() { return m_global_inputs; }
    const Lib_ray_sub_shader*     get_sub_shader_info(const string& name);

  private:
    void generate_bound_resource_desc(Lib_ray_sub_shader& sub_shader);
    void generate_cbuffer_desc(Lib_ray_sub_shader& sub_shader);

    void generate_global_input();

    Shader_reflection_info m_global_inputs;

    ComPtr<ID3D12LibraryReflection> m_lib_reflection;
    D3D12_LIBRARY_DESC              m_lib_desc;

    vector<Lib_ray_sub_shader> m_sub_shaders;
};

}; // namespace D3D12
