#pragma once

#include "vkb/VkBootstrap.h"
#include "vma/vma.h"

#include "buffer.h"
#include "frame_resource.h"

namespace Retired_code {
    class Initialization_helper;
}

namespace VKN {

    class Resource_manager;
    class Shader_manager;

    class Device {
      public:
        friend class Resource_manager;
        friend class Shader_manager;
        friend class Frame_resource;
        friend class Technique;
        friend class Technique_instance;
        friend class Shader;
        friend class Descriptor_pool;

        Device();

        void create(HINSTANCE hinstance, HWND hwnd);
        void destroy();

        void create_vma_allocator();

        void create_command_buffer();
        void create_depth_buffer();

        void create_sync_object();
        void destroy_sync_object();

        void create_descriptor_pool();

        // void draw_once();
        void draw();

        static uint32_t format_size(vk::Format format);

        void load_resources();

        void begin_single_command_submission();
        void end_single_command_submission();

        void begin_frame();
        void end_frame();

        // utilities
        vk::Format backbuffer_colour_format() const { return m_swapchain_format; }
        vk::Image backbuffer_colour_image() const { return m_swapchain_images[m_swapchain_buffer_idx]; }
        vk::ImageView backbuffer_colour_image_view() const { return m_swapchain_image_views[m_swapchain_buffer_idx]; }
        vk::Extent2D backbuffer_colour_size() const { return m_swapchain_image_size; }

        // should be part of the frame pipeline?
        vk::Format backbuffer_depth_format() const { return m_depth_buffer.m_format; }
        vk::Image backbuffer_depth_image() const { return m_depth_buffer.m_image; }

        struct Transition_image_layout_info {
            vk::ImageLayout dst_layout;
            vk::ImageLayout src_layout;
            vk::AccessFlags2 dst_access_flags;
            vk::AccessFlags2 src_access_flags;
            vk::PipelineStageFlags2 dst_stage_flags;
            vk::PipelineStageFlags2 src_stage_flags;
        };

        void transition_image_layout(vk::Image image, const Transition_image_layout_info& transition_image_layout_info);

        vk::CommandBuffer* curr_command_buffer();

        // Gfx subsystems
        std::unique_ptr<Resource_manager> m_resource_manager;
        std::unique_ptr<Shader_manager> m_shader_manager;

      private:
        void create_from_initialization_helper();
        void create_from_vk_bootstrap();

        void create_surface();

        CRect get_window_rect() const;

        std::vector<const char*> get_instance_extensions();
        std::vector<const char*> get_device_extensions();

        uint32_t curr_frame_resource_idx() const;
        Frame_resource& curr_frame_resource();

        vk::CommandBuffer m_single_use_command_buffer;

        // Resource manager
        void destroy_resource(Image& resource);

        // Window
        HINSTANCE m_hinstance;
        HWND m_hwnd;

        // Instance
        static const uint32_t m_req_api_version = VK_API_VERSION_1_3;
        vk::Instance m_vk_instance;
        // Surface
        vk::SurfaceKHR m_surface;
        // Physical/Logical device
        vk::PhysicalDevice m_physical_device;
        vk::Device m_device;
        // Queues
        uint32_t m_graphics_queue_family_index = 0;
        uint32_t m_present_queue_family_index  = 0;
        vk::Queue m_graphics_queue;
        vk::Queue m_present_queue;

        //
        const static uint32_t MAX_FRAMES_IN_FLIGHT = 2;
        uint32_t m_frame_count                     = 0;

        // Command pool
        vk::CommandPool m_command_pool;
        // vk::CommandBuffer m_command_buffer; // move into Frame_resource

        // Swapchain
        uint32_t m_swapchain_buffer_idx = 0;
        std::vector<vk::ImageView> m_swapchain_image_views;
        std::vector<vk::Image> m_swapchain_images;
        vk::SwapchainKHR m_swapchain;
        vk::Extent2D m_swapchain_image_size;
        vk::Format m_swapchain_format;

        // vkBootstrap
        vkb::Swapchain m_vkb_swapchain;

        // VMA Allocator
        vma::Allocator m_vma_allocator;

        // Buffer
        Image m_depth_buffer;

        // Syncronization
        const uint64_t m_fence_timeout = 100000000;

        std::array<std::unique_ptr<Frame_resource>, MAX_FRAMES_IN_FLIGHT> m_frame_resource;
    };

} // namespace VKN
