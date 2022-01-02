#pragma once

#include "buffer.h"
#include "descriptor_heap.h"

using Microsoft::WRL::ComPtr;

namespace D3D12 {
// Stores the resources needed for the CPU to build the command lists
// for a frame.
class FrameResource {
  public:
    FrameResource(ID3D12Device* device);

    void destroy();

    // mark some invalid operation
    FrameResource(const FrameResource& rhs) = delete;
    FrameResource& operator=(const FrameResource& rhs) = delete;

    void begin_frame()
    {
        clear_per_frame_resources();
        m_srv_heap.reset();
    }

    void clear_per_frame_resources()
    {
        for (auto&& buffer : m_per_frame_buffers) {
            buffer->destroy();
        }

        m_per_frame_buffers.clear();
    }

  public:
    Descriptor_heap m_srv_heap;

    // We cannot reset the allocator until the GPU is done processing the commands.
    // So each frame needs their own allocator.
    ComPtr<ID3D12CommandAllocator> m_command_list_allocator;

    // Fence value to mark commands up to this fence point.  This lets us
    // check if these frame resources are still in use by the GPU.
    UINT64 m_fence = 0;

    // dynamic buffers
    std::vector<shared_ptr<Buffer>> m_dynamic_buffer;

    // staging buffers
    std::list<shared_ptr<Buffer>> m_per_frame_buffers;
};
} // namespace D3D12
