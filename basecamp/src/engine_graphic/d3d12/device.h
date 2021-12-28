#pragma once

#include "D3D12MemoryAllocator/D3D12MemAlloc.h"
#include "d3dx12/d3dx12.h"

#include "command_list.h"
#include "resource.h"

using Microsoft::WRL::ComPtr;

namespace D3D12 {
class FrameResource;

class Device {
  public:
    friend class Resource_manager;

    Device();
    ~Device();

    void LoadPipeline(HWND hwnd);
    void OnResize();

    Resource_manager& resource_manager() { return *m_resource_mgr; }

    /// helper
    void begin_load_resources();
    void end_load_resources();
    void begin_frame();
    void end_frame();

    D3D12_VIEWPORT get_window_viewport() const;
    CD3DX12_RECT   get_window_rect() const;

    /// low level
    ID3D12Device5* d3d_device() { return m_device.Get(); }
    Command_list&  commmand_list() { return m_commandList; }
    FrameResource& frame_resource() { return *m_frame_resource_list[m_curr_frame_resource_index]; }

    ID3D12Resource&             curr_backbuffer() const { return *m_swap_chain_buffer[m_curr_swap_chain_buffer].Get(); }
    D3D12_CPU_DESCRIPTOR_HANDLE curr_backbuffer_view() const
    {
        return m_rtv_heap.get_cpu_descriptor(m_swap_chain_buffer_descriptor_handle_id[m_curr_swap_chain_buffer]);
    }
    D3D12_CPU_DESCRIPTOR_HANDLE curr_backbuffer_depth_stencil_view() const;

    DXGI_SWAP_CHAIN_DESC get_swap_chain_desc() const;
    DXGI_FORMAT          get_depth_stencil_format() const { return m_depth_stencil_format; }

    void wait_for_gpu();
    void flush_command_queue();

    shared_ptr<Buffer>         create_cbuffer(uint32_t size, const string& name);
    shared_ptr<Dynamic_buffer> create_dynamic_cbuffer(uint32_t size, const string& name);

    void* get_mapped_data(const Dynamic_buffer& buffer);

    std::tuple<bool, CD3DX12_GPU_DESCRIPTOR_HANDLE> get_gpu_descriptor_handle(const Dynamic_buffer& buffer);
    std::tuple<bool, CD3DX12_GPU_DESCRIPTOR_HANDLE> get_gpu_descriptor_handle(weak_ptr<Buffer> buffer_handle);
    std::tuple<bool, CD3DX12_GPU_DESCRIPTOR_HANDLE> get_uav_gpu_descriptor_handle(weak_ptr<Buffer> buffer_handle);
    std::tuple<bool, CD3DX12_GPU_DESCRIPTOR_HANDLE> get_gpu_descriptor_handle(weak_ptr<Sampler> handle);
    std::tuple<bool, CD3DX12_CPU_DESCRIPTOR_HANDLE> get_rtv_cpu_descriptor_handle(weak_ptr<Buffer> buffer_handle);
    std::tuple<bool, CD3DX12_CPU_DESCRIPTOR_HANDLE> get_dsv_cpu_descriptor_handle(weak_ptr<Buffer> buffer_handle);

    void buffer_state_transition(Buffer& buffer, D3D12_RESOURCE_STATES state_before, D3D12_RESOURCE_STATES state_after);
    void transfer_to_back_buffer(Buffer& buffer, D3D12_RESOURCE_STATES state_before);

    void imgui_init();
    void imgui_begin_frame();
    void imgui_render();
    void imgui_post_render();

  private:
    struct Descriptor_heap {
        ComPtr<ID3D12DescriptorHeap> m_descriptor_heap;

        const uint32_t m_max_descriptor     = 256;
        uint32_t       m_descriptor_size    = 0;
        uint32_t       m_next_descriptor_id = 0;

        uint32_t                      get_next_decriptor_id() { return m_next_descriptor_id++; }
        CD3DX12_CPU_DESCRIPTOR_HANDLE get_cpu_descriptor(uint32_t id) const
        {
            return CD3DX12_CPU_DESCRIPTOR_HANDLE(m_descriptor_heap->GetCPUDescriptorHandleForHeapStart(), m_descriptor_size * id);
        }
        CD3DX12_GPU_DESCRIPTOR_HANDLE get_gpu_descriptor(uint32_t id) const
        {
            return CD3DX12_GPU_DESCRIPTOR_HANDLE(m_descriptor_heap->GetGPUDescriptorHandleForHeapStart(), m_descriptor_size * id);
        }
    };

    void FindHardwareAdapter(IDXGIFactory4& factory);

    void CreateCommandObjects();
    void CreateSwapChain();

    void create_srv_descriptor_heap();
    void create_rtv_and_dsv_descriptor_heaps();
    void create_sampler_descriptor_heap();

    void reset_current_command_allocator();
    void create_backbuffer();

    // imgui
    ComPtr<ID3D12DescriptorHeap> m_imgui_srv_descriptor_heap;

    // frame resource function
    void build_frame_resource_list();

    static const uint32_t    m_num_swap_chain_buffer_count = 2;
    static const DXGI_FORMAT m_depth_stencil_format        = DXGI_FORMAT_D24_UNORM_S8_UINT;

    /// win32
    HWND m_hWnd;

    ComPtr<IDXGIFactory4> m_dxgiFactory;
    /// pipeline state
    ComPtr<IDXGIAdapter1> m_adapter;
    // DirectX Raytracing (DXR) attributes
    // ComPtr<ID3D12Device5> m_dxrDevice;
    ComPtr<ID3D12Device5> m_device;

    // memory manager
    D3D12MA::Allocator* m_allocator;

    ComPtr<ID3D12CommandQueue> m_commandQueue;
    Command_list               m_commandList;

    ComPtr<IDXGISwapChain3> m_swapChain;
    UINT                    m_curr_swap_chain_buffer = 0;

    uint64_t            m_currFence = 0;
    ComPtr<ID3D12Fence> m_fence;

    Descriptor_heap m_rtv_heap;
    Descriptor_heap m_dsv_heap;
    Descriptor_heap m_srv_heap;
    Descriptor_heap m_sampler_heap;

    ComPtr<ID3D12Resource> m_swap_chain_buffer[m_num_swap_chain_buffer_count];
    uint32_t               m_swap_chain_buffer_descriptor_handle_id[m_num_swap_chain_buffer_count];

    string m_depth_buffer_id = "default_depth_buffer";

    D3D12_VIEWPORT m_screenViewport;
    D3D12_RECT     m_screenScissorRect;

    // Frame Resources
    const static UINT                 m_num_frame_resources = 3;
    vector<unique_ptr<FrameResource>> m_frame_resource_list;
    UINT                              m_curr_frame_resource_index = 0;

    unique_ptr<Resource_manager> m_resource_mgr;
};
} // namespace D3D12
