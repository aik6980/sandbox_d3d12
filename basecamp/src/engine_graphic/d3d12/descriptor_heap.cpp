#include "descriptor_heap.h"

namespace D3D12 {
uint32_t Descriptor_heap::get_next_decriptor_id()
{
    if (m_next_descriptor_id >= m_max_descriptor) {
        // out of bound access
        throw;
    }

    return m_next_descriptor_id++;
}
} // namespace D3D12
