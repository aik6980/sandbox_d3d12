#pragma once

#include "engine_graphic.h"
#include "shader.h"

namespace D3D12 {
struct Shader;
class Technique;
class Lib_ray_technique;
struct Lib_ray_sub_technique;

class Shader_manager {
  public:
    static ComPtr<ID3DBlob> load_from_objfile(const wstring& filename);

    Shader_manager(Device& device) : m_device(device) {}

    void init();

    Device&         device() { return m_device; };
    Shader*         get_shader(const string& name);
    Lib_ray_shader* get_lib_shader(const string& name);

    weak_ptr<Technique>         get_render_technique(const string& name);
    ComPtr<ID3D12PipelineState> get_pso(weak_ptr<Technique> tech_handle, DXGI_FORMAT rt, DXGI_FORMAT ds);

    weak_ptr<Lib_ray_technique> get_lib_ray_technique(const string& name);

    void register_technique(const string& name, const TechniqueInit& init_data);
    void register_lib_ray_technique(const string& name, const string& lib_ray);

    void build_local_root_signature(Lib_ray_sub_technique& t, const Shader_reflection_info& reflection);

  private:
    void build_root_signature(Technique& t);

    void validation(Technique& t);

    unordered_map<string, unique_ptr<Shader>>         m_shader_list;
    unordered_map<string, unique_ptr<Lib_ray_shader>> m_lib_shader_list;

    unordered_map<string, shared_ptr<Technique>>       m_render_technique_list;
    unordered_map<string, ComPtr<ID3D12PipelineState>> m_pso_list;

    unordered_map<string, shared_ptr<Lib_ray_technique>> m_lib_ray_technique_list;

    Device& m_device;
};
} // namespace D3D12
