#pragma once

#include "vma/vma.h"

#include "frame_resource.h"
#include "shader.h"

namespace VKN {

struct Image {
    vk::Format      m_format;
    vk::Image       m_image;
    vma::Allocation m_alloc;
    vk::ImageView   m_view;
};

class Device {
  public:
    friend class Shader;

    Device();

    void create(HINSTANCE hinstance, HWND hwnd);
    void destroy();

    void create_instance();
    void create_device();
    void create_vma_allocator();

    void create_command_buffer();
    void create_swapchain();
    void create_depth_buffer();

    void create_render_pass();

    void create_sync_object();
    void destroy_sync_object();

    void draw_once();
    void draw();

    static uint32_t format_size(vk::Format format);

  private:
    CRect get_window_rect() const;

    std::vector<const char*> gather_layers(const std::vector<std::string>& layers, const std::vector<vk::LayerProperties>& layer_properties);
    std::vector<const char*> gather_extensions(const std::vector<std::string>& extensions, const std::vector<vk::ExtensionProperties>& extension_properties);

    vk::StructureChain<vk::InstanceCreateInfo, vk::DebugUtilsMessengerCreateInfoEXT> make_instance_create_info_chain(
        const vk::ApplicationInfo& application_info, const std::vector<const char*>& layers, const std::vector<const char*>& extensions);

    vk::DebugUtilsMessengerEXT            create_debug_utils_messenger_EXT(const vk::Instance& instance);
    vk::DebugUtilsMessengerCreateInfoEXT  make_debug_utils_messenger_create_info_EXT();
    static VKAPI_ATTR VkBool32 VKAPI_CALL debug_utils_messenger_callback(VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
        VkDebugUtilsMessageTypeFlagsEXT message_types, const VkDebugUtilsMessengerCallbackDataEXT* p_callback_data, void* /*p_user_data*/);

    uint32_t find_graphics_queue_family_index(const std::vector<vk::QueueFamilyProperties>& queue_family_properties);
    uint32_t find_present_queue_family_index();

    std::vector<std::string> get_instance_extensions();
    std::vector<std::string> get_device_extensions();

    vk::SurfaceFormatKHR pick_surface_format(std::vector<vk::SurfaceFormatKHR> const& formats);

    uint32_t curr_frame_resource_idx();

    // per Technique
    void create_pipeline_state_object();

    // Resource manager
    void destroy_resource(Image& resource);

    // Window
    HINSTANCE m_hinstance;
    HWND      m_hwnd;

    // Instance
    static const uint32_t      m_req_api_version = VK_API_VERSION_1_3;
    vk::Instance               m_instance;
    vk::DebugUtilsMessengerEXT m_debug_utils_messenger;

    vk::PhysicalDevice m_physical_device;

    vk::Device m_device;
    uint32_t   m_graphics_queue_family_index = 0;
    uint32_t   m_present_queue_family_index  = 0;

    //
    const static uint32_t MAX_FRAMES_IN_FLIGHT = 2;
    uint32_t              m_frame_count        = 0;

    // Command pool
    vk::CommandPool m_command_pool;
    // vk::CommandBuffer m_command_buffer; // move into Frame_resource

    // Window surface
    vk::SurfaceKHR             m_surface;
    std::vector<vk::ImageView> m_swapchain_image_views;
    vk::SwapchainKHR           m_swapchain;
    vk::Extent2D               m_swapchain_image_size;

    // VMA Allocator
    vma::Allocator m_vma_allocator;

    // Buffer
    Image m_depth_buffer;

    // Render pass/Frame buffer (for swapchain)
    vk::RenderPass               m_render_pass;
    std::vector<vk::Framebuffer> m_frame_buffers;

    // Render pipeline
    vk::Pipeline m_pipeline;

    // Syncronization
    const uint64_t m_fence_timeout = 100000000;

    // vk::Semaphore m_image_available_semaphore;
    // vk::Semaphore m_render_finished_semaphore;
    // vk::Fence     m_inflight_fence;

    // Sub system
    Shader m_vertex_shader;
    Shader m_vertex_shader_2;
    Shader m_pixel_shader;

    std::array<Frame_resource, MAX_FRAMES_IN_FLIGHT> m_frame_resource;
};
} // namespace VKN
