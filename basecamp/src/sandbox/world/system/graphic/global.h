#pragma once

#include "engine_graphic/engine_graphic.h"

class Engine;

extern string g_shadowmap_depth_buffer_id;

// sampler
extern string g_point_sampler_id;

template <class T>
string build_mesh(const vector<T>& mesh_verts, const MeshIndexArray& mesh_indices, const string& name, Engine& engine)
{
    // upload data to the GPU
    auto&& vtx_data = mesh_verts.data();
    auto&& vtx_size = mesh_verts.size() * sizeof(mesh_verts[0]);
    auto&& vb       = engine.resource_mgr().create_static_buffer(name + "_vb", (uint32_t)vtx_size, vtx_data);

    auto&& idx_data = mesh_indices.m_indices32.data();
    auto&& idx_size = mesh_indices.m_indices32.size() * sizeof(mesh_indices.m_indices32[0]);
    auto&& ib       = engine.resource_mgr().create_static_buffer(name + "_ib", (uint32_t)idx_size, idx_data);

    auto&& mesh_buffer                  = std::make_shared<D3D12::Mesh_buffer>();
    mesh_buffer->m_vertex_buffer_handle = vb;
    mesh_buffer->m_index_buffer_handle  = ib;

    mesh_buffer->ib_bytes_size    = (uint32_t)idx_size;
    mesh_buffer->idx_format       = DXGI_FORMAT_R32_UINT;
    mesh_buffer->vb_bytes_size    = (uint32_t)vtx_size;
    mesh_buffer->vtx_bytes_stride = sizeof(mesh_verts[0]);

    D3D12::MESH_LOCATION loc;
    loc.index_count              = (uint32_t)mesh_indices.m_indices32.size();
    mesh_buffer->m_mesh_location = loc;

    engine.resource_mgr().register_mesh_buffer(name, mesh_buffer);

    return name;
}
