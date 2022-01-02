

#include "shader_reflection.h"
#include "common/common.cpp.h"
#include "dxc/dxcapi.h"

namespace D3D12 {

BYTE build_component_mask(BYTE num)
{
    return (1 << num) - 1;
}

DXGI_FORMAT ShaderReflection::map_to_dxgi_format(D3D_REGISTER_COMPONENT_TYPE component_type, BYTE mask)
{
    switch (component_type) {
    case D3D_REGISTER_COMPONENT_FLOAT32:
        if (mask & build_component_mask(4))
            return DXGI_FORMAT_R32G32B32A32_FLOAT;
        else if (mask & build_component_mask(3))
            return DXGI_FORMAT_R32G32B32_FLOAT;
        else if (mask & build_component_mask(2))
            return DXGI_FORMAT_R32G32_FLOAT;
        else if (mask & build_component_mask(1))
            return DXGI_FORMAT_R32_FLOAT;
        else {
            throw;
            return DXGI_FORMAT_UNKNOWN;
        }

    case D3D_REGISTER_COMPONENT_UINT32:
        if (mask & build_component_mask(4))
            return DXGI_FORMAT_R32G32B32A32_UINT;
        else if (mask & build_component_mask(3))
            return DXGI_FORMAT_R32G32B32_UINT;
        else if (mask & build_component_mask(2))
            return DXGI_FORMAT_R32G32_UINT;
        else if (mask & build_component_mask(1))
            return DXGI_FORMAT_R32_UINT;
        else {
            throw;
            return DXGI_FORMAT_UNKNOWN;
        }

    case D3D_REGISTER_COMPONENT_SINT32:
        if (mask & build_component_mask(4))
            return DXGI_FORMAT_R32G32B32A32_SINT;
        else if (mask & build_component_mask(3))
            return DXGI_FORMAT_R32G32B32_SINT;
        else if (mask & build_component_mask(2))
            return DXGI_FORMAT_R32G32_SINT;
        else if (mask & build_component_mask(1))
            return DXGI_FORMAT_R32_SINT;
        else {
            throw;
            return DXGI_FORMAT_UNKNOWN;
        }
    }

    assert(false);
    return DXGI_FORMAT_UNKNOWN;
}

D3D12_INPUT_ELEMENT_DESC ShaderReflection::map_to_d3d_input_element_desc(const D3D12_SIGNATURE_PARAMETER_DESC& src, bool is_instance_data)
{
    D3D12_INPUT_ELEMENT_DESC output;
    output.SemanticName         = src.SemanticName;
    output.SemanticIndex        = src.SemanticIndex;
    output.Format               = map_to_dxgi_format(src.ComponentType, src.Mask);
    output.InputSlot            = is_instance_data ? 1 : 0;
    output.AlignedByteOffset    = D3D12_APPEND_ALIGNED_ELEMENT;
    output.InputSlotClass       = is_instance_data ? D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA : D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
    output.InstanceDataStepRate = is_instance_data ? 1 : 0;

    return output;
}

constexpr uint32_t make_four_cc(uint8_t a, uint8_t b, uint8_t c, uint8_t d)
{
    return a | b << 8 | c << 16 | d << 24;
}

void ShaderReflection::get_reflection(ComPtr<ID3DBlob> buffer)
{
    // using DXC as our shader compiler
    ComPtr<IDxcContainerReflection> dxc_reflection;
    DxcCreateInstance(CLSID_DxcContainerReflection, IID_PPV_ARGS(&dxc_reflection));
    DBG::throw_hr(dxc_reflection->Load((IDxcBlob*)buffer.Get()));

    uint32_t part_index;
    auto&&   dxil_fourcc = make_four_cc('D', 'X', 'I', 'L');
    DBG::throw_hr(dxc_reflection->FindFirstPartKind(dxil_fourcc, &part_index));

    ComPtr<ID3D12ShaderReflection> reflection;
    DBG::throw_hr(dxc_reflection->GetPartReflection(part_index, IID_PPV_ARGS(&reflection)));

    m_shader_reflection = reflection;
    m_shader_reflection->GetDesc(&m_shader_desc);

    // input layout desc
    generate_input_layout_desc();

    // get shader info
    generate_bound_resource_desc();
    generate_cbuffer_desc();
    generate_output_parameter_desc();
}

void ShaderReflection::generate_input_layout_desc()
{
    auto&& num_input_parameters = m_shader_desc.InputParameters;

    for (uint32_t i = 0; i < num_input_parameters; ++i) {
        D3D12_SIGNATURE_PARAMETER_DESC desc;
        m_shader_reflection->GetInputParameterDesc(i, &desc);

        D3D12_INPUT_ELEMENT_DESC input_element_desc;
        if (Case_insensitive_find_substr(string(desc.SemanticName), string("Instance")) != -1) {
            input_element_desc = map_to_d3d_input_element_desc(desc, true);
        }
        else {
            input_element_desc = map_to_d3d_input_element_desc(desc, false);
        }
        m_input_layout_desc.add_layout_element(input_element_desc);
    }
}

void ShaderReflection::generate_output_parameter_desc()
{
    auto&& num_parameters = m_shader_desc.OutputParameters;

    for (uint32_t i = 0; i < num_parameters; ++i) {
        D3D12_SIGNATURE_PARAMETER_DESC desc;
        m_shader_reflection->GetOutputParameterDesc(i, &desc);

        assert(&desc);
    }
}

string to_string(LPCSTR cstr)
{
    return (cstr != 0) ? cstr : "";
}

void ShaderReflection::generate_cbuffer_desc()
{
    auto&& cbuffer_desc = m_infos.m_shader_input_descs[Sit_cbuffer];

    for (uint32_t i = 0; i < cbuffer_desc.size(); ++i) {
        auto&& name                       = cbuffer_desc[i].Name;
        auto&& constant_buffer_reflection = m_shader_reflection->GetConstantBufferByName(name);

        Cbuffer_info cbuffer_info;
        constant_buffer_reflection->GetDesc(&cbuffer_info.m_desc);
        cbuffer_info.m_name = to_string(cbuffer_info.m_desc.Name);

        for (uint32_t j = 0; j < cbuffer_info.m_desc.Variables; ++j) {
            CBUFFER_VARIABLE_INFO var_info;
            auto&&                d3d_var_info = constant_buffer_reflection->GetVariableByIndex(j);

            d3d_var_info->GetDesc(&var_info.m_desc);
            var_info.m_name = to_string(var_info.m_desc.Name);

            cbuffer_info.m_variable_infos.emplace(std::make_pair(var_info.m_name, var_info));
        }

        m_infos.m_cbuffer_infos.emplace(std::make_pair(cbuffer_info.m_name, cbuffer_info));
    }
}

void ShaderReflection::generate_bound_resource_desc()
{
    auto&& num_items = m_shader_desc.BoundResources;

    for (uint32_t i = 0; i < num_items; ++i) {
        D3D12_SHADER_INPUT_BIND_DESC desc;
        m_shader_reflection->GetResourceBindingDesc(i, &desc);

        m_infos.generate_bound_resource_desc(desc);
    }
}

Lib_ray_reflection::Lib_ray_reflection() {}

void Lib_ray_reflection::get_reflection(ComPtr<ID3DBlob> buffer)
{
    // using DXC as our shader compiler
    ComPtr<IDxcContainerReflection> dxc_reflection;
    DxcCreateInstance(CLSID_DxcContainerReflection, IID_PPV_ARGS(&dxc_reflection));
    DBG::throw_hr(dxc_reflection->Load((IDxcBlob*)buffer.Get()));

    uint32_t part_index;
    auto&&   dxil_fourcc = make_four_cc('D', 'X', 'I', 'L');
    DBG::throw_hr(dxc_reflection->FindFirstPartKind(dxil_fourcc, &part_index));

    ComPtr<ID3D12LibraryReflection> reflection;
    DBG::throw_hr(dxc_reflection->GetPartReflection(part_index, IID_PPV_ARGS(&reflection)));

    m_lib_reflection = reflection;
    m_lib_reflection->GetDesc(&m_lib_desc);

    // allocate sub objects
    m_sub_shaders.resize(m_lib_desc.FunctionCount);

    for (int i = 0; i < m_lib_desc.FunctionCount; ++i) {
        auto&&              func_reflection = m_lib_reflection->GetFunctionByIndex(i);
        D3D12_FUNCTION_DESC func_desc;
        func_reflection->GetDesc(&func_desc);

        m_sub_shaders[i].m_func_reflection = func_reflection;
        m_sub_shaders[i].m_func_desc       = func_desc;

        // get function name from mangling text eg. "/x1?closethit_entry@@YAXUPayload_st@@UBuiltInTriangleIntersectionAttributes@@@Z"
        string mangling_name = func_desc.Name;
        auto&& beg_pos       = mangling_name.find("?");
        // exclude the token itself
        beg_pos += 1;
        auto&& end_pos          = mangling_name.find("@@");
        auto&& func_name        = mangling_name.substr(beg_pos, end_pos - beg_pos);
        m_sub_shaders[i].m_name = func_name;

        generate_bound_resource_desc(m_sub_shaders[i]);
        generate_cbuffer_desc(m_sub_shaders[i]);
    }

    // generate global input desc, we will use this to create global root signature
    generate_global_input();
}
const Lib_ray_sub_shader* Lib_ray_reflection::get_sub_shader_info(const string& name)
{
    auto matched_name_func = [&name](const Lib_ray_sub_shader& sub_shader) { return sub_shader.m_name == name; };

    auto found = std::find_if(m_sub_shaders.cbegin(), m_sub_shaders.cend(), matched_name_func);
    if (found != m_sub_shaders.end()) {
        return &(*found);
    }

    return nullptr;
}
void Lib_ray_reflection::generate_bound_resource_desc(Lib_ray_sub_shader& sub_shader)
{
    D3D12_FUNCTION_DESC& func_desc = sub_shader.m_func_desc;
    auto&&               num_items = func_desc.BoundResources;

    for (uint32_t i = 0; i < num_items; ++i) {
        D3D12_SHADER_INPUT_BIND_DESC desc;
        sub_shader.m_func_reflection->GetResourceBindingDesc(i, &desc);

        sub_shader.m_func_input_info.generate_bound_resource_desc(desc);
    }
}

void Lib_ray_reflection::generate_cbuffer_desc(Lib_ray_sub_shader& sub_shader)
{
    auto&& var_infos       = sub_shader.m_func_input_info;
    auto&& func_reflection = sub_shader.m_func_reflection;

    auto&& cbuffer_desc = var_infos.m_shader_input_descs[Sit_cbuffer];

    for (uint32_t i = 0; i < cbuffer_desc.size(); ++i) {
        auto&& name                       = cbuffer_desc[i].Name;
        auto&& constant_buffer_reflection = func_reflection->GetConstantBufferByName(name);

        Cbuffer_info cbuffer_info;
        constant_buffer_reflection->GetDesc(&cbuffer_info.m_desc);
        cbuffer_info.m_name = to_string(cbuffer_info.m_desc.Name);

        for (uint32_t j = 0; j < cbuffer_info.m_desc.Variables; ++j) {
            CBUFFER_VARIABLE_INFO var_info;
            auto&&                d3d_var_info = constant_buffer_reflection->GetVariableByIndex(j);

            d3d_var_info->GetDesc(&var_info.m_desc);
            var_info.m_name = to_string(var_info.m_desc.Name);

            cbuffer_info.m_variable_infos.emplace(std::make_pair(var_info.m_name, var_info));
        }

        var_infos.m_cbuffer_infos.emplace(std::make_pair(cbuffer_info.m_name, cbuffer_info));
    }
}

void Lib_ray_reflection::generate_global_input()
{
    array<uint32_t, Sit_count> shader_input_desc_size = {};

    // calculate size needed
    for (auto&& sub_shader : m_sub_shaders) {

        for (int i = 0; i < Sit_count; ++i) {
            shader_input_desc_size[i] += sub_shader.m_func_input_info.m_shader_input_descs[i].size();
        }
    }

    // reserve size
    for (int i = 0; i < Sit_count; ++i) {
        m_global_inputs.m_shader_input_descs[i].reserve(shader_input_desc_size[i]);
    }

    // copy into global array
    for (auto&& sub_shader : m_sub_shaders) {

        for (int i = 0; i < Sit_count; ++i) {

            auto&& dest    = m_global_inputs.m_shader_input_descs[i].end();
            auto&& src_beg = sub_shader.m_func_input_info.m_shader_input_descs[i].begin();
            auto&& src_end = sub_shader.m_func_input_info.m_shader_input_descs[i].end();

            m_global_inputs.m_shader_input_descs[i].insert(dest, src_beg, src_end);
        }

        auto&& src_beg = sub_shader.m_func_input_info.m_cbuffer_infos.begin();
        auto&& src_end = sub_shader.m_func_input_info.m_cbuffer_infos.end();
        m_global_inputs.m_cbuffer_infos.insert(src_beg, src_end);
    }

    // sorting by binding id
    auto sort_func   = [](const D3D12_SHADER_INPUT_BIND_DESC& a, const D3D12_SHADER_INPUT_BIND_DESC& b) { return a.BindPoint < b.BindPoint; };
    auto unique_func = [](const D3D12_SHADER_INPUT_BIND_DESC& a, const D3D12_SHADER_INPUT_BIND_DESC& b) { return strcmp(a.Name, b.Name) == 0; };

    for (int i = 0; i < Sit_count; ++i) {
        auto&& beg = m_global_inputs.m_shader_input_descs[i].begin();
        auto&& end = m_global_inputs.m_shader_input_descs[i].end();

        std::sort(beg, end, sort_func);
        // remove duplicated items in the list
        auto unique_end = std::unique(beg, end, unique_func);
        m_global_inputs.m_shader_input_descs[i].erase(unique_end, end);
    }
}

// [Note]
// D3D_SIT_RTACCELERATIONSTRUCTURE will be used as an SRV
void Shader_reflection_info::generate_bound_resource_desc(const D3D12_SHADER_INPUT_BIND_DESC& desc)
{
    switch (desc.Type) {
    case D3D_SIT_CBUFFER:
        m_shader_input_descs[Sit_cbuffer].emplace_back(desc);
        break;
    case D3D_SIT_TEXTURE:
    case D3D_SIT_RTACCELERATIONSTRUCTURE:
        m_shader_input_descs[Sit_srv].emplace_back(desc);
        break;
    case D3D_SIT_UAV_RWTYPED:
    case D3D_SIT_UAV_RWSTRUCTURED:
        m_shader_input_descs[Sit_uav].emplace_back(desc);
        break;
    case D3D_SIT_SAMPLER:
        m_shader_input_descs[Sit_sampler].emplace_back(desc);
        break;
    default:
        throw;
        break;
    }
}
} // namespace D3D12
