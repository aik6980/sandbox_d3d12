

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

D3D12_INPUT_ELEMENT_DESC ShaderReflection::map_to_d3d_input_element_desc(const D3D12_SIGNATURE_PARAMETER_DESC &src)
{
    D3D12_INPUT_ELEMENT_DESC output;
    output.SemanticName = src.SemanticName;
    output.SemanticIndex = src.SemanticIndex;
    output.Format = map_to_dxgi_format(src.ComponentType, src.Mask);
    output.InputSlot = 0;
    output.AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
    output.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
    output.InstanceDataStepRate = 0;

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
    DBG::throw_hr(dxc_reflection->Load((IDxcBlob *)buffer.Get()));

    uint32_t part_index;
    auto &&dxil_fourcc = make_four_cc('D', 'X', 'I', 'L');
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
    auto &&num_input_parameters = m_shader_desc.InputParameters;

    for (uint32_t i = 0; i < num_input_parameters; ++i) {
        D3D12_SIGNATURE_PARAMETER_DESC desc;
        m_shader_reflection->GetInputParameterDesc(i, &desc);

        D3D12_INPUT_ELEMENT_DESC input_element_desc = map_to_d3d_input_element_desc(desc);
        m_input_layout_desc.add_layout_element(input_element_desc);
    }
}

void ShaderReflection::generate_output_parameter_desc()
{
    auto &&num_parameters = m_shader_desc.OutputParameters;

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
    for (uint32_t i = 0; i < m_cbuffer_desc.size(); ++i) {
        auto &&name = m_cbuffer_desc[i].Name;
        auto &&constant_buffer_reflection = m_shader_reflection->GetConstantBufferByName(name);

        CBUFFER_INFO cbuffer_info;
        constant_buffer_reflection->GetDesc(&cbuffer_info.m_desc);
        cbuffer_info.m_name = to_string(cbuffer_info.m_desc.Name);

        for (uint32_t j = 0; j < cbuffer_info.m_desc.Variables; ++j) {
            CBUFFER_VARIABLE_INFO var_info;
            auto &&d3d_var_info = constant_buffer_reflection->GetVariableByIndex(j);

            d3d_var_info->GetDesc(&var_info.m_desc);
            var_info.m_name = to_string(var_info.m_desc.Name);

            cbuffer_info.m_variable_infos.emplace(std::make_pair(var_info.m_name, var_info));
        }

        m_cbuffer_infos.emplace(std::make_pair(cbuffer_info.m_name, cbuffer_info));
    }
}

void ShaderReflection::generate_bound_resource_desc()
{
    auto &&num_items = m_shader_desc.BoundResources;

    for (uint32_t i = 0; i < num_items; ++i) {
        D3D12_SHADER_INPUT_BIND_DESC desc;
        m_shader_reflection->GetResourceBindingDesc(i, &desc);

        switch (desc.Type) {
        case D3D_SIT_CBUFFER:
            m_cbuffer_desc.emplace_back(desc);
            break;
        case D3D_SIT_TEXTURE:
            m_srv_desc.emplace_back(desc);
            break;
        case D3D_SIT_UAV_RWTYPED:
        case D3D_SIT_UAV_RWSTRUCTURED:
            m_uav_desc.emplace_back(desc);
            break;
        case D3D_SIT_SAMPLER:
            m_sampler_desc.emplace_back(desc);
            break;
        default:
            throw;
            break;
        }
    }
}
} // namespace D3D12
