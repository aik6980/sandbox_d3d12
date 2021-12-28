#pragma once

using Microsoft::WRL::ComPtr;

namespace D3D12 {
class Command_list {
  public:
    // ID3D12GraphicsCommandList4 for dxr
    ID3D12GraphicsCommandList4* operator()() { return m_obj.Get(); }

    void init(ComPtr<ID3D12GraphicsCommandList4> com_ptr);
    void reset(ID3D12CommandAllocator* command_allocator, ID3D12PipelineState* pipeline_state);
    void close();

    bool is_openned() { return m_is_opened; }

  private:
    // Dxr Command list
    // ComPtr<ID3D12GraphicsCommandList4> m_dxrCommandList;
    ComPtr<ID3D12GraphicsCommandList4> m_obj;
    bool                               m_is_opened = false;
};
} // namespace D3D12
