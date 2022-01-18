#pragma once

#include "buffer.h"

namespace D3D12 {

struct MESH_LOCATION {
    uint32_t index_count          = 0;
    uint32_t start_index_location = 0;
    uint32_t base_vertex_location = 0;
};

class Mesh_buffer {
  public:
    std::weak_ptr<Buffer> m_vertex_buffer_handle;
    std::weak_ptr<Buffer> m_index_buffer_handle;

    MESH_LOCATION m_mesh_location;

    // Data about the buffers.
    uint32_t               vtx_bytes_stride = 0;
    uint32_t               vb_bytes_size    = 0;
    DXGI_FORMAT            idx_format       = DXGI_FORMAT_R32_UINT;
    uint32_t               ib_bytes_size    = 0;
    D3D_PRIMITIVE_TOPOLOGY prim_topology    = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

    D3D12_VERTEX_BUFFER_VIEW vertex_buffer_view() const
    {
        D3D12_VERTEX_BUFFER_VIEW view = {};

        if (auto buffer = m_vertex_buffer_handle.lock()) {
            view.BufferLocation = buffer->m_buffer->GetGPUVirtualAddress();
            view.SizeInBytes    = vb_bytes_size;
            view.StrideInBytes  = vtx_bytes_stride;
        }
        else {
            throw;
        }

        return view;
    }

    D3D12_INDEX_BUFFER_VIEW index_buffer_view() const
    {
        D3D12_INDEX_BUFFER_VIEW view = {};

        if (auto buffer = m_index_buffer_handle.lock()) {
            view.BufferLocation = buffer->m_buffer->GetGPUVirtualAddress();
            view.Format         = idx_format;
            view.SizeInBytes    = ib_bytes_size;
        }
        else {
            throw;
        }

        return view;
    }
};
} // namespace D3D12
