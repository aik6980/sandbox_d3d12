#include "scene_buffer.h"

namespace D3D12 {
void Scene_data::add_instance(const string& mesh_name, const XMMATRIX& transform)
{
    auto&& found = m_instance_transforms.find(mesh_name);
    // not found, add a new container
    if (found == m_instance_transforms.end()) {
        m_instance_transforms[mesh_name] = vector<XMMATRIX>();
    }

    // add to the back
    m_instance_transforms[mesh_name].emplace_back(transform);
}

uint32_t Scene_data::num_instances() const
{
    uint32_t total = 0;
    for (auto&& obj : m_instance_transforms) {
        total += obj.second.size();
    }

    return total;
}

} // namespace D3D12
