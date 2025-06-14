#pragma once

#include "buffer.h"

namespace VKN {

    class Device;

    class Resource_manager {
      public:
        Resource_manager(Device& gfx_device)
            : m_gfx_device(gfx_device)
        {
        }

        void destroy();

        // test function
        void create_mesh();

        void destroy_buffer(Buffer& buffer);

        Buffer create_buffer(const Buffer_create_info& create_info);
        Buffer create_constant_buffer(const void* src_data, size_t size);

        void create_texture(const TextureData& texture_data);

        Buffer m_vertex_buffer;
        Buffer m_index_buffer;

      private:
        Device& m_gfx_device;

        std::vector<Buffer> m_staging_buffers;
    };

} // namespace VKN
