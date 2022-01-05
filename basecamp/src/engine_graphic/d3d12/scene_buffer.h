#pragma once

namespace D3D12 {

struct Scene_data {
    std::unordered_map<string, vector<XMMATRIX>> m_instance_transforms;

    void     add_instance(const string& mesh_name, const XMMATRIX& transform);
    uint32_t num_instances() const;
};

} // namespace D3D12
