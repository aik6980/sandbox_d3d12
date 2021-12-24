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

class Lib_ray_technique {
  public:
    Lib_ray_technique(Shader_manager& shader_mgr) : m_shader_mgr(shader_mgr) {}

    const CBUFFER_VARIABLE_INFO* get_cbuffer_var_info(const string& cbuffer_name, const string& var_name);
    const INPUT_LAYOUT_DESC*     get_input_layout_desc();
    Graphics_pipeline_state_desc get_graphic_pipeline_state_desc();

    string m_vs;
    string m_ps;
    string m_lib_ray;

    ComPtr<ID3D12RootSignature> m_root_signature;

    vector<CD3DX12_ROOT_PARAMETER>               m_root_parameter_slots;
    vector<unique_ptr<CD3DX12_DESCRIPTOR_RANGE>> m_descriptor_ranges;
    vector<string>                               m_descriptor_table_names;

  private:
    const CBUFFER_INFO*          get_cbuffer_info(const string& shader_name, const string& cbuffer_name);
    const CBUFFER_VARIABLE_INFO* get_cbuffer_var_info(const string& shader_name, const string& cbuffer_name, const string& var_name);

    Shader_manager& m_shader_mgr;
};

} // namespace D3D12
