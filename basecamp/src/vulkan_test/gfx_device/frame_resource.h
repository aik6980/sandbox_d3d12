#pragma once

#include "descriptor_pool.h"

namespace VKN {

    class Device;
    struct Buffer;
    class Frame_resource {
      public:
        Frame_resource(Device& gfx_device)
            : m_gfx_device(gfx_device)
            , m_descriptor_pool(gfx_device)
        {
        }

        void destroy_resources();

        void begin_frame();
        void end_frame();

        vk::CommandBuffer m_command_buffer;
        bool              m_command_buffer_opened = false;

        vk::Semaphore m_image_available_semaphore;
        vk::Semaphore m_render_finished_semaphore;
        vk::Fence     m_inflight_fence;

        // descriptor pool per flight
        Descriptor_pool m_descriptor_pool;

        // per flight resources
        std::vector<std::shared_ptr<Buffer>> m_buffers;

      private:
        Device& m_gfx_device;
    };

} // namespace VKN
