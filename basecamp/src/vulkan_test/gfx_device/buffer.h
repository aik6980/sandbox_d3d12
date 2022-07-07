#pragma once

#include "vma/vma.h"

namespace VKN {

    struct Buffer_create_info {
        vk::BufferUsageFlagBits m_usage_flags;

        const void* m_data = nullptr;
        size_t      m_size;
    };

    class Buffer {
      public:
        vk::Buffer      m_buffer;
        vma::Allocation m_allocation;
    };

    struct Image {
        vk::Format      m_format;
        vk::Image       m_image;
        vma::Allocation m_alloc;
        vk::ImageView   m_view;
    };

    struct Mesh_buffer {
        std::unique_ptr<Buffer> m_vb;
        std::unique_ptr<Buffer> m_ib;
    };

} // namespace VKN
