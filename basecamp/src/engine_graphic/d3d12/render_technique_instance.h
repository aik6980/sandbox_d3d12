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

class Technique {
  public:
    Technique(Shader_manager& shader_mgr) : m_shader_mgr(shader_mgr) {}

    const CBUFFER_VARIABLE_INFO* get_cbuffer_var_info(const string& cbuffer_name, const string& var_name);
    const INPUT_LAYOUT_DESC*     get_input_layout_desc();

    Graphics_pipeline_state_desc get_graphic_pipeline_state_desc();
    Compute_pipeline_state_desc  get_compute_pipeline_state_desc();

    string m_vs;
    string m_ps;
    string m_cs;

    ComPtr<ID3D12RootSignature> m_root_signature;

    vector<CD3DX12_ROOT_PARAMETER>               m_root_parameter_slots;
    vector<unique_ptr<CD3DX12_DESCRIPTOR_RANGE>> m_descriptor_ranges;
    vector<string>                               m_descriptor_table_names;

  private:
    const Cbuffer_info*          get_cbuffer_info(const string& shader_name, const string& cbuffer_name);
    const CBUFFER_VARIABLE_INFO* get_cbuffer_var_info(const string& shader_name, const string& cbuffer_name, const string& var_name);

    Shader_manager& m_shader_mgr;
};

class TechniqueInstance {
  public:
    TechniqueInstance(Device& device, Shader_manager& shader_mgr) : m_device(device), m_shader_mgr(shader_mgr) {}

    void init(const string& technique_name);

    weak_ptr<Technique> get_technique() { return m_technique_handle; }

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
    void set_raster_root_signature_parameters(ID3D12GraphicsCommandList& command_list);
    void set_compute_root_signature_parameters(ID3D12GraphicsCommandList& command_list);

    void init_dynamic_cbuffer(const string& shader_name);

    Device&         m_device;
    Shader_manager& m_shader_mgr;

    weak_ptr<Technique> m_technique_handle;

    unordered_map<string, shared_ptr<Dynamic_buffer>> m_cbuffer;
    unordered_map<string, weak_ptr<Buffer>>           m_srv;
    unordered_map<string, weak_ptr<Buffer>>           m_uav;
    unordered_map<string, weak_ptr<Sampler>>          m_samplers;
};
} // namespace D3D12
