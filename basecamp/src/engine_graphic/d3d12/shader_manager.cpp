

#include "shader_manager.h"
#include "common/common_cpp.h"
#include "device.h"
#include "graphic_pipeline_state_desc.h"
#include "lib_ray_technique_instance.h"
#include "render_technique_instance.h"
#include "shader.h"
#include "shader_reflection.h"

namespace D3D12 {
ComPtr<ID3DBlob> Shader_manager::load_from_objfile(const wstring& filename)
{
    ComPtr<ID3DBlob> bytecode;
    DBG::throw_hr(D3DReadFileToBlob(filename.c_str(), &bytecode));

    return bytecode;
}

void Shader_manager::init() {}

Shader* Shader_manager::get_shader(const string& name)
{
    if (name == "") {
        return nullptr;
    }

    auto&& found = m_shader_list.find(name);
    if (found != m_shader_list.end()) {
        return found->second.get();
    }

    wstring w_name    = To_wstring(name);
    wstring file_path = L"shader_hlsl/" + w_name + L".obj";

    auto&& shader_obj = std::make_unique<Shader>();

    shader_obj->m_buffer = load_from_objfile(file_path);
    auto&& reflection    = std::make_unique<ShaderReflection>();

    reflection->get_reflection(shader_obj->m_buffer);

    shader_obj->m_reflection = std::move(reflection);

    m_shader_list[name] = std::move(shader_obj);

    return m_shader_list[name].get();
}

Lib_ray_shader* Shader_manager::get_lib_shader(const string& name)
{
    if (name == "") {
        return nullptr;
    }

    auto&& found = m_lib_shader_list.find(name);
    if (found != m_lib_shader_list.end()) {
        return found->second.get();
    }

    wstring w_name    = To_wstring(name);
    wstring file_path = L"shader_hlsl/" + w_name + L".obj";

    auto&& shader_obj = std::make_unique<Lib_ray_shader>();

    shader_obj->m_buffer = load_from_objfile(file_path);
    auto&& reflection    = std::make_unique<Lib_ray_reflection>();

    reflection->get_reflection(shader_obj->m_buffer);

    shader_obj->m_reflection = std::move(reflection);

    m_lib_shader_list[name] = std::move(shader_obj);

    return m_lib_shader_list[name].get();
}

weak_ptr<Technique> Shader_manager::get_render_technique(const string& name)
{
    auto&& found = m_render_technique_list.find(name);
    if (found != m_render_technique_list.end()) {
        return found->second;
    }

    return weak_ptr<Technique>();
}

ComPtr<ID3D12PipelineState> Shader_manager::get_pso(Technique& tech, DXGI_FORMAT rt, DXGI_FORMAT ds)
{
    string key    = DBG::Format("%#16x%d%d", &tech, rt, ds);
    auto&& result = m_pso_list.find(key);

    if (result != m_pso_list.end()) {
        return result->second;
    }

    ComPtr<ID3D12PipelineState> pso;
    if (!tech.m_vs.empty()) {
        auto&& pso_desc           = tech.get_graphic_pipeline_state_desc();
        pso_desc.NumRenderTargets = (rt != DXGI_FORMAT_UNKNOWN) ? 1 : 0;
        pso_desc.RTVFormats[0]    = rt;
        pso_desc.DSVFormat        = ds;

        DBG::throw_hr(m_device.d3d_device()->CreateGraphicsPipelineState(&pso_desc, IID_PPV_ARGS(&pso)));
    }
    else if (!tech.m_cs.empty()) {
        auto&& pso_desc = tech.get_compute_pipeline_state_desc();

        DBG::throw_hr(m_device.d3d_device()->CreateComputePipelineState(&pso_desc, IID_PPV_ARGS(&pso)));
    }
    else {
        throw;
    }

    m_pso_list[key] = pso;
    return pso;
}

unique_ptr<Technique_instance> Shader_manager::create_technique_instance(const string& technique_name, DXGI_FORMAT rt, DXGI_FORMAT ds)
{
    auto&& technique = get_render_technique(technique_name).lock();
    if (technique) {
        auto&& instance = std::make_unique<Technique_instance>(m_device, *this);
        instance->init(technique_name);

        auto&& pso      = get_pso(*technique, rt, ds);
        instance->m_pso = pso;

        return std::move(instance);
    }

    return nullptr;
}

weak_ptr<Lib_ray_technique> Shader_manager::get_lib_ray_technique(const string& name)
{
    auto&& found = m_lib_ray_technique_list.find(name);
    if (found != m_lib_ray_technique_list.end()) {
        return found->second;
    }

    return weak_ptr<Lib_ray_technique>();
}

unique_ptr<Lib_ray_technique_instance> Shader_manager::create_lib_ray_technique_instance(const string& technique_name)
{
    auto&& technique = get_lib_ray_technique(technique_name).lock();
    if (technique) {
        auto&& instance = std::make_unique<Lib_ray_technique_instance>(m_device, *this);
        instance->init(technique_name);

        return std::move(instance);
    }

    return nullptr;
}

void Shader_manager::register_technique(const string& name, const TechniqueInit& init_data)
{
    auto&& t = std::make_shared<Technique>(*this);
    t->m_vs  = init_data.m_vs;
    t->m_ps  = init_data.m_ps;
    t->m_cs  = init_data.m_cs;

    build_root_signature(*t);

    m_render_technique_list[name] = t;
}

void Shader_manager::register_lib_ray_technique(const string& name, const string& lib_ray)
{
    auto&& t = std::make_shared<Lib_ray_technique>(*this);
    t->m_lib = lib_ray;

    t->create_ray_tracing_pipeline_state_object();
    t->create_shader_table();

    m_lib_ray_technique_list[name] = t;
}

// need refactoring
void append_root_parameter_slot(vector<CD3DX12_ROOT_PARAMETER>& root_parameter_slots, vector<unique_ptr<CD3DX12_DESCRIPTOR_RANGE>>& descriptor_ranges,
    vector<string>& descriptor_table_names, const D3D12::Shader_reflection_info& reflection, D3D12_SHADER_VISIBILITY visibility)
{
    auto&& sh       = reflection;
    auto&& sh_state = visibility;

    auto&& num_cbuffer = sh.cbuffer_binding_desc().size();
    auto&& num_srv     = sh.srv_binding_desc().size();
    auto&& num_uav     = sh.uav_binding_desc().size();
    auto&& num_sampler = sh.sampler_binding_desc().size();

    auto append_root_parameter_func = [&root_parameter_slots, &descriptor_ranges, &descriptor_table_names](
                                          int id, const string& name, D3D12_DESCRIPTOR_RANGE_TYPE range_type, D3D12_SHADER_VISIBILITY shader_visibility) {
        auto&& tbl_item = std::make_unique<CD3DX12_DESCRIPTOR_RANGE>();
        tbl_item->Init(range_type, 1, id);

        CD3DX12_ROOT_PARAMETER root_parameter;
        root_parameter.InitAsDescriptorTable(1, tbl_item.get(), shader_visibility);

        descriptor_ranges.emplace_back(std::move(tbl_item));
        root_parameter_slots.emplace_back(root_parameter);
        descriptor_table_names.emplace_back(name);
    };

    for (int i = 0; i < num_cbuffer; ++i) {
        auto&& name = sh.cbuffer_binding_desc()[i].Name;
        append_root_parameter_func(i, name, D3D12_DESCRIPTOR_RANGE_TYPE_CBV, visibility);
    }

    for (int i = 0; i < num_srv; ++i) {
        auto&& name = sh.srv_binding_desc()[i].Name;
        append_root_parameter_func(i, name, D3D12_DESCRIPTOR_RANGE_TYPE_SRV, visibility);
    }

    for (int i = 0; i < num_uav; ++i) {
        auto&& name = sh.uav_binding_desc()[i].Name;
        append_root_parameter_func(i, name, D3D12_DESCRIPTOR_RANGE_TYPE_UAV, visibility);
    }

    for (int i = 0; i < num_sampler; ++i) {
        auto&& name = sh.sampler_binding_desc()[i].Name;
        append_root_parameter_func(i, name, D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER, visibility);
    }
}

void Shader_manager::build_root_signature(Technique& t)
{
    auto&& vs = get_shader(t.m_vs);
    auto&& ps = get_shader(t.m_ps);
    auto&& cs = get_shader(t.m_cs);

    auto&& root_parameter_slots   = t.m_root_parameter_slots;
    auto&& descriptor_ranges      = t.m_descriptor_ranges;
    auto&& descriptor_table_names = t.m_descriptor_table_names;

    if (vs) {
        append_root_parameter_slot(
            root_parameter_slots, descriptor_ranges, descriptor_table_names, vs->m_reflection->get_infos(), D3D12_SHADER_VISIBILITY_VERTEX);
    }
    if (ps) {
        append_root_parameter_slot(
            root_parameter_slots, descriptor_ranges, descriptor_table_names, ps->m_reflection->get_infos(), D3D12_SHADER_VISIBILITY_PIXEL);
    }
    if (cs) {
        append_root_parameter_slot(root_parameter_slots, descriptor_ranges, descriptor_table_names, cs->m_reflection->get_infos(), D3D12_SHADER_VISIBILITY_ALL);
    }

    // A root signature is an array of root parameters.
    auto&& flags = cs ? D3D12_ROOT_SIGNATURE_FLAG_NONE : D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

    CD3DX12_ROOT_SIGNATURE_DESC root_sig_desc((UINT)root_parameter_slots.size(), root_parameter_slots.data(), 0, nullptr, flags);

    ComPtr<ID3DBlob> serialized_root_sig = nullptr;
    ComPtr<ID3DBlob> error_blob          = nullptr;
    DBG::test_hr(D3D12SerializeRootSignature(&root_sig_desc, D3D_ROOT_SIGNATURE_VERSION_1, &serialized_root_sig, &error_blob));

    if (error_blob != nullptr) {
        DBG::OutputString((char*)error_blob->GetBufferPointer());
    }

    auto&& render_device = m_device;
    DBG::throw_hr(render_device.d3d_device()->CreateRootSignature(
        0, serialized_root_sig->GetBufferPointer(), serialized_root_sig->GetBufferSize(), IID_PPV_ARGS(&t.m_root_signature)));

    // validation
    validation(t);
}

// [Note] global and local function are exactly the same, please refactoring this!
void Shader_manager::build_global_root_signature(Lib_ray_technique& t, const Shader_reflection_info& reflection)
{
    auto&& root_parameter_slots   = t.m_root_parameter_slots;
    auto&& descriptor_ranges      = t.m_descriptor_ranges;
    auto&& descriptor_table_names = t.m_descriptor_table_names;

    append_root_parameter_slot(root_parameter_slots, descriptor_ranges, descriptor_table_names, reflection, D3D12_SHADER_VISIBILITY_ALL);

    // A root signature is an array of root parameters.
    // auto&& flags = cs ? D3D12_ROOT_SIGNATURE_FLAG_NONE : D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
    auto&& flags = D3D12_ROOT_SIGNATURE_FLAG_NONE;

    CD3DX12_ROOT_SIGNATURE_DESC root_sig_desc((UINT)root_parameter_slots.size(), root_parameter_slots.data(), 0, nullptr, flags);

    ComPtr<ID3DBlob> serialized_root_sig = nullptr;
    ComPtr<ID3DBlob> error_blob          = nullptr;
    DBG::test_hr(D3D12SerializeRootSignature(&root_sig_desc, D3D_ROOT_SIGNATURE_VERSION_1, &serialized_root_sig, &error_blob));

    if (error_blob != nullptr) {
        DBG::OutputString((char*)error_blob->GetBufferPointer());
    }

    auto&& render_device = m_device;
    DBG::throw_hr(render_device.d3d_device()->CreateRootSignature(
        0, serialized_root_sig->GetBufferPointer(), serialized_root_sig->GetBufferSize(), IID_PPV_ARGS(&t.m_root_signature)));

    // validation
    // validation(t);
}

void Shader_manager::build_local_root_signature(Lib_ray_sub_technique& t, const Shader_reflection_info& reflection)
{
    auto&& root_parameter_slots   = t.m_root_parameter_slots;
    auto&& descriptor_ranges      = t.m_descriptor_ranges;
    auto&& descriptor_table_names = t.m_descriptor_table_names;

    append_root_parameter_slot(root_parameter_slots, descriptor_ranges, descriptor_table_names, reflection, D3D12_SHADER_VISIBILITY_ALL);

    // A root signature is an array of root parameters.
    // auto&& flags = cs ? D3D12_ROOT_SIGNATURE_FLAG_NONE : D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
    auto&& flags = D3D12_ROOT_SIGNATURE_FLAG_LOCAL_ROOT_SIGNATURE;

    CD3DX12_ROOT_SIGNATURE_DESC root_sig_desc((UINT)root_parameter_slots.size(), root_parameter_slots.data(), 0, nullptr, flags);

    ComPtr<ID3DBlob> serialized_root_sig = nullptr;
    ComPtr<ID3DBlob> error_blob          = nullptr;
    DBG::test_hr(D3D12SerializeRootSignature(&root_sig_desc, D3D_ROOT_SIGNATURE_VERSION_1, &serialized_root_sig, &error_blob));

    if (error_blob != nullptr) {
        DBG::OutputString((char*)error_blob->GetBufferPointer());
    }

    auto&& render_device = m_device;
    DBG::throw_hr(render_device.d3d_device()->CreateRootSignature(
        0, serialized_root_sig->GetBufferPointer(), serialized_root_sig->GetBufferSize(), IID_PPV_ARGS(&t.m_root_signature)));

    // validation
    // validation(t);
}

void Shader_manager::validation(Technique& t)
{
    auto&& root_parameter_slots   = t.m_root_parameter_slots;
    auto&& descriptor_table_names = t.m_descriptor_table_names;
    auto&& descriptor_ranges      = t.m_descriptor_ranges;

    auto&& valid = true;
    for (uint32_t i = 0; i < descriptor_table_names.size(); ++i) {
        auto&& name_a          = descriptor_table_names[i];
        auto&& resource_type_a = descriptor_ranges[i]->RangeType;

        for (uint32_t j = i + 1; j < descriptor_table_names.size(); ++j) {
            auto&& name_b           = descriptor_table_names[j];
            auto&& resource_type_b  = descriptor_ranges[j]->RangeType;
            auto&& shader_visibilty = root_parameter_slots[j].ShaderVisibility;

            // if 2 shader resources have the same name, they need to be an exact match
            if (name_a == name_b) {
                if (resource_type_a != resource_type_b) {
                    valid = false;
                    DBG::OutputString("error: ambigious resource name found: %s \n", name_a);
                }
                else {
                }
            }
        }
    }

    if (!valid) {
        throw;
    }
}

} // namespace D3D12
