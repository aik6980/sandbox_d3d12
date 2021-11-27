

#include "shader_manager.h"
#include "common/common.cpp.h"
#include "device.h"
#include "graphic_pipeline_state_desc.h"
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

    // Example converting string <=> wstring
    // std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    // std::string narrow = converter.to_bytes(wide_utf16_source_string);
    // std::wstring wide = converter.from_bytes(narrow_utf8_source_string);

    wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;

    wstring w_name    = converter.from_bytes(name);
    wstring file_path = L"shader_hlsl/" + w_name + L".obj";

    auto&& shader_obj = std::make_unique<Shader>();

    shader_obj->m_buffer = load_from_objfile(file_path);
    auto&& reflection    = std::make_unique<ShaderReflection>();
    reflection->get_reflection(shader_obj->m_buffer);
    shader_obj->m_reflection = std::move(reflection);

    m_shader_list[name] = std::move(shader_obj);

    return m_shader_list[name].get();
}

weak_ptr<Technique> Shader_manager::get_render_technique(const string& name)
{
    auto&& found = m_render_technique_list.find(name);
    if (found != m_render_technique_list.end()) {
        return found->second;
    }

    return weak_ptr<Technique>();
}

ComPtr<ID3D12PipelineState> Shader_manager::get_pso(weak_ptr<Technique> tech_handle, DXGI_FORMAT rt, DXGI_FORMAT ds)
{
    auto&& tech = tech_handle.lock();
    if (tech) {

        string key    = DBG::Format("%#16x%d%d", tech.get(), rt, ds);
        auto&& result = m_pso_list.find(key);

        if (result != m_pso_list.end()) {
            return result->second;
        }

        auto&& pso_desc           = tech->get_graphic_pipeline_state_desc();
        pso_desc.NumRenderTargets = (rt != DXGI_FORMAT_UNKNOWN) ? 1 : 0;
        pso_desc.RTVFormats[0]    = rt;
        pso_desc.DSVFormat        = ds;

        ComPtr<ID3D12PipelineState> pso;
        DBG::throw_hr(m_device.device()->CreateGraphicsPipelineState(&pso_desc, IID_PPV_ARGS(&pso)));

        m_pso_list[key] = pso;
        return pso;
    }

    return nullptr;
}

void Shader_manager::register_technique(const string& name, const TechniqueInit& init_data)
{
    auto&& t = std::make_shared<Technique>(*this);
    t->m_vs  = init_data.m_vs;
    t->m_ps  = init_data.m_ps;

    build_root_signature(*t);

    m_render_technique_list[name] = t;
}

// need refactoring
void append_root_parameter_slot(vector<CD3DX12_ROOT_PARAMETER>& root_parameter_slots, vector<unique_ptr<CD3DX12_DESCRIPTOR_RANGE>>& descriptor_ranges, vector<string>& descriptor_table_names, const D3D12::ShaderReflection& reflection, D3D12_SHADER_VISIBILITY visibility)
{
    auto&& sh       = reflection;
    auto&& sh_state = visibility;

    auto&& num_cbuffer = sh.cbuffer_binding_desc().size();
    auto&& num_srv     = sh.srv_binding_desc().size();
    auto&& num_uav     = sh.uav_binding_desc().size();
    auto&& num_sampler = sh.sampler_binding_desc().size();

    auto append_root_parameter_func = [&root_parameter_slots, &descriptor_ranges, &descriptor_table_names](int id, const string& name, D3D12_DESCRIPTOR_RANGE_TYPE range_type, D3D12_SHADER_VISIBILITY shader_visibility) {
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

    auto&& root_parameter_slots   = t.m_root_parameter_slots;
    auto&& descriptor_ranges      = t.m_descriptor_ranges;
    auto&& descriptor_table_names = t.m_descriptor_table_names;

    append_root_parameter_slot(root_parameter_slots, descriptor_ranges, descriptor_table_names, *(vs->m_reflection), D3D12_SHADER_VISIBILITY_VERTEX);

    if (ps) {
        append_root_parameter_slot(root_parameter_slots, descriptor_ranges, descriptor_table_names, *(ps->m_reflection), D3D12_SHADER_VISIBILITY_PIXEL);
    }

    // A root signature is an array of root parameters.
    CD3DX12_ROOT_SIGNATURE_DESC root_sig_desc((UINT)root_parameter_slots.size(), root_parameter_slots.data(), 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

    ComPtr<ID3DBlob> serialized_root_sig = nullptr;
    ComPtr<ID3DBlob> error_blob          = nullptr;
    DBG::test_hr(D3D12SerializeRootSignature(&root_sig_desc, D3D_ROOT_SIGNATURE_VERSION_1, &serialized_root_sig, &error_blob));

    if (error_blob != nullptr) {
        DBG::OutputString((char*)error_blob->GetBufferPointer());
    }

    auto&& render_device = m_device;
    DBG::throw_hr(render_device.device()->CreateRootSignature(0, serialized_root_sig->GetBufferPointer(), serialized_root_sig->GetBufferSize(), IID_PPV_ARGS(&t.m_root_signature)));

    // validation
    validation(t);
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
