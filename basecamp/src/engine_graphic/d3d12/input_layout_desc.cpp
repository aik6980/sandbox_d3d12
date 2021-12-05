#include "input_layout_desc.h"

namespace D3D12 {
D3D12_INPUT_LAYOUT_DESC INPUT_LAYOUT_DESC::as_d3d12_input_layout_desc() const
{
    if (!valid()) {
        return {nullptr, 0};
    }

    return {m_input_layout.data(), (UINT)m_input_layout.size()};
}

void INPUT_LAYOUT_DESC::add_layout_element(const string& name)
{
    m_semantic_names.emplace_back(name);

    D3D12_INPUT_ELEMENT_DESC desc;
    desc.SemanticName         = m_semantic_names.back().c_str();
    desc.SemanticIndex        = 0;
    desc.Format               = DXGI_FORMAT_R32G32B32_FLOAT;
    desc.InputSlot            = 0;
    desc.AlignedByteOffset    = 0;
    desc.InputSlotClass       = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
    desc.InstanceDataStepRate = 0;

    m_input_layout.emplace_back(desc);
}

void INPUT_LAYOUT_DESC::add_layout_element(const D3D12_INPUT_ELEMENT_DESC& input)
{
    m_semantic_names.emplace_back(input.SemanticName);

    D3D12_INPUT_ELEMENT_DESC desc = input;
    // patching name string address
    desc.SemanticName = m_semantic_names.back().c_str();

    m_input_layout.emplace_back(desc);
}
} // namespace D3D12
