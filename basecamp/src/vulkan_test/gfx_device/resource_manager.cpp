#include "resource_manager.h"

#include "common/common_cpp.h"

#include "device.h"
#include "vma/vma.h"

namespace VKN {
    void Resource_manager::destroy()
    {
        destroy_buffer(m_vertex_buffer);
        destroy_buffer(m_index_buffer);

        for (auto&& buffer : m_staging_buffers) {
            destroy_buffer(buffer);
        }
    }

    void Resource_manager::create_mesh()
    {
        auto&& mesh = MeshDataGenerator::create_unit_cube();

        // create vertex buffer
        auto&& vb_data = MeshDataGenerator::to_p1c1(mesh.m_vertices);
        auto&& vb =
            create_buffer({.m_usage_flags = vk::BufferUsageFlagBits::eVertexBuffer, .m_data = vb_data.data(), .m_size = vb_data.size() * sizeof(vb_data[0])});

        // create index buffer
        auto&& ib_data = mesh.m_indices.m_indices32;
        auto&& ib =
            create_buffer({.m_usage_flags = vk::BufferUsageFlagBits::eIndexBuffer, .m_data = ib_data.data(), .m_size = ib_data.size() * sizeof(ib_data[0])});

        m_vertex_buffer = vb;
        m_index_buffer  = ib;
    }

    void Resource_manager::destroy_buffer(Buffer& buffer)
    {
        m_gfx_device.m_device.destroyBuffer(buffer.m_buffer);
        m_gfx_device.m_vma_allocator.freeMemory(buffer.m_allocation);
    }

    Buffer Resource_manager::create_buffer(const Buffer_create_info& create_info)
    {
        auto&& usage_flags = create_info.m_usage_flags;
        auto&& data        = create_info.m_data;
        auto&& size        = create_info.m_size;

        auto&& vma_allocator  = m_gfx_device.m_vma_allocator;
        auto&& command_buffer = m_gfx_device.m_single_use_command_buffer;

        // create staging buffer
        vk::Buffer      staging_buffer;
        vma::Allocation staging_buffer_alloc;
        {
            auto&& create_info = vk::BufferCreateInfo{
                .size        = size,
                .usage       = vk::BufferUsageFlagBits::eTransferSrc,
                .sharingMode = vk::SharingMode::eExclusive,
            };
            auto&& alloc_create_info = vma::AllocationCreateInfo();
            alloc_create_info.setUsage(vma::MemoryUsage::eAuto);
            alloc_create_info.setFlags(vma::AllocationCreateFlagBits::eHostAccessSequentialWrite | vma::AllocationCreateFlagBits::eMapped);

            vma::AllocationInfo buffer_alloc_info;

            std::tie(staging_buffer, staging_buffer_alloc) = vma_allocator.createBuffer(create_info, alloc_create_info, buffer_alloc_info);

            // copy src data
            std::memcpy(buffer_alloc_info.pMappedData, data, size);

            m_staging_buffers.emplace_back(Buffer{.m_buffer = staging_buffer, .m_allocation = staging_buffer_alloc});
        }

        vk::Buffer      buffer;
        vma::Allocation buffer_alloc;
        {
            auto&& create_info = vk::BufferCreateInfo{
                .size        = size,
                .usage       = vk::BufferUsageFlagBits::eTransferDst | usage_flags,
                .sharingMode = vk::SharingMode::eExclusive,
            };
            auto&& alloc_create_info = vma::AllocationCreateInfo();

            std::tie(buffer, buffer_alloc) = vma_allocator.createBuffer(create_info, alloc_create_info);
        }

        // copy buffer
        auto&& copy_region = vk::BufferCopy(0, 0, size);
        command_buffer.copyBuffer(staging_buffer, buffer, copy_region);

        return Buffer{.m_buffer = buffer, .m_allocation = buffer_alloc};
    }

    Buffer Resource_manager::create_constant_buffer(const void* src_data, size_t size)
    {
        auto&& vma_allocator = m_gfx_device.m_vma_allocator;

        auto&& usage_flags = vk::BufferUsageFlagBits::eUniformBuffer;

        auto&& create_info = vk::BufferCreateInfo{
            .size        = size,
            .usage       = vk::BufferUsageFlagBits::eTransferSrc | usage_flags,
            .sharingMode = vk::SharingMode::eExclusive,
        };

        auto&& alloc_create_info = vma::AllocationCreateInfo();
        alloc_create_info.setUsage(vma::MemoryUsage::eAuto);
        alloc_create_info.setFlags(vma::AllocationCreateFlagBits::eHostAccessSequentialWrite | vma::AllocationCreateFlagBits::eMapped);

        vk::Buffer          buffer;
        vma::Allocation     buffer_alloc;
        vma::AllocationInfo buffer_alloc_info;
        std::tie(buffer, buffer_alloc) = vma_allocator.createBuffer(create_info, alloc_create_info, buffer_alloc_info);

        // copy src data
        std::memcpy(buffer_alloc_info.pMappedData, src_data, size);

        return Buffer{.m_buffer = buffer, .m_allocation = buffer_alloc, .m_size = buffer_alloc_info.size};
    }

} // namespace VKN
