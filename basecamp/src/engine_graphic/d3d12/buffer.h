#pragma once

#include "D3D12MemoryAllocator/D3D12MemAlloc.h"
#include "sampler_desc.h"

namespace D3D12 {

struct Buffer_request {
    CD3DX12_RESOURCE_DESC desc;
    bool                  lifetime_persistent = true;
};

struct Buffer {
    D3D12MA::Allocation*   m_allocation = nullptr;
    ComPtr<ID3D12Resource> m_buffer;

    D3D12_RESOURCE_DESC m_d3d_desc;
    // d3d descriptor id (pointer to view in the heap)
    // uint32_t m_cbv_srv_handle_id = 0; <- get dynamically from frame resource
    // uint32_t m_uav_handle_id     = 0; <- get dynamically from frame resource
    uint32_t m_rtv_handle_id = 0;
    uint32_t m_dsv_handle_id = 0;

    // state
    D3D12_RESOURCE_STATES m_curr_state = D3D12_RESOURCE_STATE_COMMON;

    void destroy()
    {
        m_buffer.Reset();
        if (m_allocation) {
            m_allocation->Release();
            m_allocation = nullptr;
        }
    }

    ~Buffer() { destroy(); }
};

struct Dynamic_buffer {
    vector<weak_ptr<Buffer>> m_buffer;
    vector<void*>            m_data;
};

struct Sampler {
    D3d12x_sampler_desc m_d3d_desc;
    uint32_t            m_handle_id = 0;
};
} // namespace D3D12
