#pragma once

#include "resource.h"

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

    void clear_staging_resources()
    {
        for (auto&& buffer : m_staging_buffers) {
            buffer->destroy();
        }

        m_staging_buffers.clear();
    }

  public:
    // We cannot reset the allocator until the GPU is done processing the commands.
    // So each frame needs their own allocator.
    ComPtr<ID3D12CommandAllocator> m_command_list_allocator;

    // Fence value to mark commands up to this fence point.  This lets us
    // check if these frame resources are still in use by the GPU.
    UINT64 m_fence = 0;

    // dynamic buffers
    std::vector<shared_ptr<Buffer>> m_dynamic_buffer;

    // staging buffers
    std::list<shared_ptr<Buffer>> m_staging_buffers;
};
} // namespace D3D12
