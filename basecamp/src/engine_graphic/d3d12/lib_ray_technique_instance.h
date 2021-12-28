#pragma once

#include "graphic_pipeline_state_desc.h"

namespace D3D12 {
class Device;
class Shader_manager;
struct CBUFFER_INFO;
struct CBUFFER_VARIABLE_INFO;
struct Buffer;
struct Sampler;
struct Dynamic_buffer;
struct INPUT_LAYOUT_DESC;

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

    void create_ray_tracing_pipeline_state_object();
    void create_shader_table();

    ComPtr<ID3D12StateObject> m_dxr_state_object;

    // shader table
    string m_raygen_shader_table_buffer;
    string m_miss_shader_table_buffer;
    string m_hitgroup_shader_table_buffer;

    Lib_ray_sub_technique m_sub_techniques;

  private:
    Shader_manager& m_shader_mgr;
};

class Lib_ray_technique_instance {
};

} // namespace D3D12
