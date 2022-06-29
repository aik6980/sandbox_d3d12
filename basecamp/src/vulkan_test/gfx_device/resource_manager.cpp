#include "resource_manager.h"

#include "vma/vma.h"

namespace VKN {

void Resource_manager::create_vertex_buffer(uint32_t byte_size)
{
    auto&& createinfo =
        vk::BufferCreateInfo({}, byte_size, vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eTransferDst, vk::SharingMode::eExclusive);
    auto&& allocinfo = vma::AllocationCreateInfo();
}
} // namespace VKN
