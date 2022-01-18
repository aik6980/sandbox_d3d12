#pragma once

#include "graphic_pipeline_state_desc.h"

namespace D3D12 {
class Device;
class Shader_manager;
struct Cbuffer_info;
struct CBUFFER_VARIABLE_INFO;
struct Buffer;
struct Sampler;
struct Dynamic_buffer;
struct INPUT_LAYOUT_DESC;
class Lib_ray_reflection;

struct Lib_ray_sub_technique {
    ComPtr<ID3D12RootSignature> m_root_signature;

    vector<CD3DX12_ROOT_PARAMETER>               m_root_parameter_slots;
    vector<unique_ptr<CD3DX12_DESCRIPTOR_RANGE>> m_descriptor_ranges;
    vector<string>                               m_descriptor_table_names;
};

class Lib_ray_technique {
  public:
    Lib_ray_technique(Shader_manager& shader_mgr) : m_shader_mgr(shader_mgr) {}

    string m_lib;

    // create pipeline object
    void create_ray_tracing_pipeline_state_object();
    void create_shader_table();
    void prepare_cbuffer_bindings();

    void create_root_signature(CD3DX12_STATE_OBJECT_DESC& raytrace_pso, Lib_ray_reflection& reflection);
    void create_root_signature_subobject(
        Lib_ray_sub_technique& sub_technique, CD3DX12_STATE_OBJECT_DESC& raytrace_pso, Lib_ray_reflection& reflection, const string& name);

    ComPtr<ID3D12StateObject> m_dxr_state_object;

    // shader table
    string m_raygen_shader_table_buffer;
    string m_miss_shader_table_buffer;
    string m_hitgroup_shader_table_buffer;

    // global
    ComPtr<ID3D12RootSignature>                  m_root_signature;
    vector<CD3DX12_ROOT_PARAMETER>               m_root_parameter_slots;
    vector<unique_ptr<CD3DX12_DESCRIPTOR_RANGE>> m_descriptor_ranges;
    vector<string>                               m_descriptor_table_names;

    // unique cbuffer infos;
    vector<const Cbuffer_info*> m_cbuffer_infos;

    // sub object (Local root signature)
    Lib_ray_sub_technique m_raygen_sub_technique;
    Lib_ray_sub_technique m_miss_sub_technique;
    Lib_ray_sub_technique m_closethit_sub_technique;

  private:
    Shader_manager& m_shader_mgr;
};

class Lib_ray_technique_instance {
  public:
    Lib_ray_technique_instance(Device& device, Shader_manager& shader_mgr) : m_device(device), m_shader_mgr(shader_mgr) {}

    void init(const string& technique_name);

    weak_ptr<Lib_ray_technique> get_technique() { return m_technique_handle; }

    template <class T>
    void set_cbv(const string& cbuffer_name, const string& var_name, const T& data)
    {
        set_cbv(cbuffer_name, var_name, (void*)&data, sizeof(data));
    }

    void set_cbv(const string& cbuffer_name, const string& var_name, void* data, uint32_t data_size);
    void set_srv(const string& var_name, weak_ptr<Buffer> buffer);
    void set_uav(const string& var_name, weak_ptr<Buffer> buffer);
    void set_sampler(const string& var_name, weak_ptr<Sampler> resource);

    void set_root_signature_parameters(ID3D12GraphicsCommandList& command_list);

  private:
    void init_cbuffer(const string& name);

    const D3D12_SHADER_VARIABLE_DESC* get_cbuffer_var_info(const string& cbuffer_name, const string& var_name);
    void                              set_root_signature_parameters(ID3D12GraphicsCommandList& command_list, Lib_ray_technique& technique);

    Device&         m_device;
    Shader_manager& m_shader_mgr;

    weak_ptr<Lib_ray_technique> m_technique_handle;

    // unordered_map<string, shared_ptr<Dynamic_buffer>> m_cbuffer;
    unordered_map<string, tuple<weak_ptr<Buffer>, void*>> m_cbuffer;
    unordered_map<string, const Cbuffer_info*>            m_cbuffer_infos;
    unordered_map<string, weak_ptr<Buffer>>               m_srv;
    unordered_map<string, weak_ptr<Buffer>>               m_uav;
    unordered_map<string, weak_ptr<Sampler>>              m_samplers;
};

} // namespace D3D12
