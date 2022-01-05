#include "frame_resource.h"
#include "common/common_cpp.h"

namespace D3D12 {
FrameResource::FrameResource(ID3D12Device* device)
{
    DBG::throw_hr(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(m_command_list_allocator.GetAddressOf())));
}

void FrameResource::destroy()
{
    for (auto&& dynamic_buffer : m_dynamic_buffer) {
        dynamic_buffer->destroy();
    }

    clear_per_frame_resources();
}
} // namespace D3D12
