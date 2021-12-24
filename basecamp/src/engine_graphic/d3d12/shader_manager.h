#pragma once

#include "engine_graphic.h"
#include "shader.h"

namespace D3D12 {
struct Shader;
class Technique;

class Shader_manager {
  public:
    static ComPtr<ID3DBlob> load_from_objfile(const wstring& filename);

    Shader_manager(Device& device) : m_device(device) {}

    void init();

    Shader*                     get_shader(const string& name);
    weak_ptr<Technique>         get_render_technique(const string& name);
    ComPtr<ID3D12PipelineState> get_pso(weak_ptr<Technique> tech_handle, DXGI_FORMAT rt, DXGI_FORMAT ds);

    void register_technique(const string& name, const TechniqueInit& init_data);
    void register_lib_ray_technique(const string& name, const string& lib_ray);

  private:
    void build_root_signature(Technique& t);
    void validation(Technique& t);

    unordered_map<string, unique_ptr<Shader>>          m_shader_list;
    unordered_map<string, shared_ptr<Technique>>       m_render_technique_list;
    unordered_map<string, ComPtr<ID3D12PipelineState>> m_pso_list;

    Device& m_device;
};
} // namespace D3D12
