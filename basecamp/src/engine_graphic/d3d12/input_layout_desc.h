#pragma once

#include "input_layout_element_desc_consts.h"

namespace D3D12 {
struct INPUT_LAYOUT_DESC {
    vector<D3D12_INPUT_ELEMENT_DESC> m_input_layout;
    // using list<> here so we prevent reallocation when call emplace_back()
    list<string> m_semantic_names;

    D3D12_INPUT_LAYOUT_DESC as_d3d12_input_layout_desc() const;

    void add_layout_element(const string& name);
    void add_layout_element(const D3D12_INPUT_ELEMENT_DESC& input);

    bool valid() const { return !m_input_layout.empty(); }
};
} // namespace D3D12
