#pragma once

namespace D3D12 {
struct Descriptor_heap {
    ComPtr<ID3D12DescriptorHeap> m_descriptor_heap;

    const uint32_t m_max_descriptor     = 256;
    uint32_t       m_descriptor_size    = 0;
    uint32_t       m_next_descriptor_id = 0;

    void                          reset() { m_next_descriptor_id = 0; }
    uint32_t                      get_next_decriptor_id();
    CD3DX12_CPU_DESCRIPTOR_HANDLE get_cpu_descriptor(uint32_t id) const
    {
        return CD3DX12_CPU_DESCRIPTOR_HANDLE(m_descriptor_heap->GetCPUDescriptorHandleForHeapStart(), m_descriptor_size * id);
    }
    CD3DX12_GPU_DESCRIPTOR_HANDLE get_gpu_descriptor(uint32_t id) const
    {
        return CD3DX12_GPU_DESCRIPTOR_HANDLE(m_descriptor_heap->GetGPUDescriptorHandleForHeapStart(), m_descriptor_size * id);
    }
};
} // namespace D3D12
