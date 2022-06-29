#pragma once

namespace VKN {
class Frame_resource {
  public:
    vk::CommandBuffer m_command_buffer;

    vk::Semaphore m_image_available_semaphore;
    vk::Semaphore m_render_finished_semaphore;
    vk::Fence     m_inflight_fence;
};

} // namespace VKN
