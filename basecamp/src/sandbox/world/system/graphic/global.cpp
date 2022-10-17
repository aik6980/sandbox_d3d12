#include "global.h"

#include "common/common_cpp.h"
#include "engine_graphic/engine.h"
#include "engine_graphic/engine_graphic_cpp.h"

string g_shadowmap_depth_buffer_id = "shadowmap_depth_buffer";

string g_point_sampler_id = "point_sampler";

string build_mesh(const void* vtx_data, const uint32_t vtx_stride, const uint32_t vtx_count, const void* idx_data, const uint32_t idx_stride, const uint32_t idx_count, const string& name, Engine& engine)
{
    // upload data to the GPU
    auto&& vtx_size = vtx_count * vtx_stride;
    auto&& vb       = engine.resource_mgr().create_static_buffer(name + "_vb", (uint32_t)vtx_size, vtx_data);

    auto&& idx_size = idx_count * idx_stride;
    auto&& ib       = engine.resource_mgr().create_static_buffer(name + "_ib", (uint32_t)idx_size, idx_data);

    auto&& mesh_buffer                  = std::make_shared<D3D12::Mesh_buffer>();
    mesh_buffer->m_vertex_buffer_handle = vb;
    mesh_buffer->m_index_buffer_handle  = ib;

    mesh_buffer->ib_bytes_size    = (uint32_t)idx_size;
    mesh_buffer->idx_format       = DXGI_FORMAT_R32_UINT;
    mesh_buffer->vb_bytes_size    = (uint32_t)vtx_size;
    mesh_buffer->vtx_bytes_stride = vtx_stride;

    D3D12::MESH_LOCATION loc;
    loc.index_count              = idx_count;
    mesh_buffer->m_mesh_location = loc;

    engine.resource_mgr().register_mesh_buffer(name, mesh_buffer);

    return name;
}
