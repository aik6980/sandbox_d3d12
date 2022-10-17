#pragma once

#include "common/common.h"
#include "engine_graphic/engine_graphic.h"

class Engine;

extern string g_shadowmap_depth_buffer_id;

// sampler
extern string g_point_sampler_id;

extern string build_mesh(const void* vtx_data, const uint32_t vtx_stride, const uint32_t vtx_count, const void* idx_data, const uint32_t idx_stride, const uint32_t idx_count, const string& name, Engine& engine);

template <class T>
string build_mesh(const vector<T>& mesh_verts, const Mesh_index_array& mesh_indices, const string& name, Engine& engine)
{
    return build_mesh(mesh_verts.data(), sizeof(mesh_verts[0]), mesh_verts.size(), mesh_indices.m_indices32.data(), sizeof(mesh_indices.m_indices32[0]), mesh_indices.m_indices32.size(), name, engine);
}

