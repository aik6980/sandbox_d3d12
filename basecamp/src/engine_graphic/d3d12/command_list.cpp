#include "command_list.h"
#include "common/common.cpp.h"

namespace D3D12 {
void Command_list::init(ComPtr<ID3D12GraphicsCommandList4> com_ptr)
{
    m_obj = com_ptr;
    DBG::throw_hr(m_obj->Close());
}

void Command_list::reset(ID3D12CommandAllocator* command_allocator, ID3D12PipelineState* pipeline_state)
{
    if (!m_is_opened) {
        DBG::throw_hr(m_obj->Reset(command_allocator, pipeline_state));
        m_is_opened = true;
    }
}

void Command_list::close()
{
    if (m_is_opened) {
        DBG::throw_hr(m_obj->Close());
        m_is_opened = false;
    }
}
} // namespace D3D12
