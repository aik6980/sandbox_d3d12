#include "device.h"

#include <thread>

#include "common/common_cpp.h"
#include "resource_manager.h"
#include "shader_manager.h"
#include "technique_instance.h"

// retired code moved here - use for references only!
#include "retired_code/initialization_helper.h"

VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE

namespace VKN {

    uint32_t Device::format_size(vk::Format format)
    {
        uint32_t result = 0;
        switch (static_cast<VkFormat>(format)) {
        case VK_FORMAT_UNDEFINED:
            result = 0;
            break;
        case VK_FORMAT_R4G4_UNORM_PACK8:
            result = 1;
            break;
        case VK_FORMAT_R4G4B4A4_UNORM_PACK16:
            result = 2;
            break;
        case VK_FORMAT_B4G4R4A4_UNORM_PACK16:
            result = 2;
            break;
        case VK_FORMAT_R5G6B5_UNORM_PACK16:
            result = 2;
            break;
        case VK_FORMAT_B5G6R5_UNORM_PACK16:
            result = 2;
            break;
        case VK_FORMAT_R5G5B5A1_UNORM_PACK16:
            result = 2;
            break;
        case VK_FORMAT_B5G5R5A1_UNORM_PACK16:
            result = 2;
            break;
        case VK_FORMAT_A1R5G5B5_UNORM_PACK16:
            result = 2;
            break;
        case VK_FORMAT_R8_UNORM:
            result = 1;
            break;
        case VK_FORMAT_R8_SNORM:
            result = 1;
            break;
        case VK_FORMAT_R8_USCALED:
            result = 1;
            break;
        case VK_FORMAT_R8_SSCALED:
            result = 1;
            break;
        case VK_FORMAT_R8_UINT:
            result = 1;
            break;
        case VK_FORMAT_R8_SINT:
            result = 1;
            break;
        case VK_FORMAT_R8_SRGB:
            result = 1;
            break;
        case VK_FORMAT_R8G8_UNORM:
            result = 2;
            break;
        case VK_FORMAT_R8G8_SNORM:
            result = 2;
            break;
        case VK_FORMAT_R8G8_USCALED:
            result = 2;
            break;
        case VK_FORMAT_R8G8_SSCALED:
            result = 2;
            break;
        case VK_FORMAT_R8G8_UINT:
            result = 2;
            break;
        case VK_FORMAT_R8G8_SINT:
            result = 2;
            break;
        case VK_FORMAT_R8G8_SRGB:
            result = 2;
            break;
        case VK_FORMAT_R8G8B8_UNORM:
            result = 3;
            break;
        case VK_FORMAT_R8G8B8_SNORM:
            result = 3;
            break;
        case VK_FORMAT_R8G8B8_USCALED:
            result = 3;
            break;
        case VK_FORMAT_R8G8B8_SSCALED:
            result = 3;
            break;
        case VK_FORMAT_R8G8B8_UINT:
            result = 3;
            break;
        case VK_FORMAT_R8G8B8_SINT:
            result = 3;
            break;
        case VK_FORMAT_R8G8B8_SRGB:
            result = 3;
            break;
        case VK_FORMAT_B8G8R8_UNORM:
            result = 3;
            break;
        case VK_FORMAT_B8G8R8_SNORM:
            result = 3;
            break;
        case VK_FORMAT_B8G8R8_USCALED:
            result = 3;
            break;
        case VK_FORMAT_B8G8R8_SSCALED:
            result = 3;
            break;
        case VK_FORMAT_B8G8R8_UINT:
            result = 3;
            break;
        case VK_FORMAT_B8G8R8_SINT:
            result = 3;
            break;
        case VK_FORMAT_B8G8R8_SRGB:
            result = 3;
            break;
        case VK_FORMAT_R8G8B8A8_UNORM:
            result = 4;
            break;
        case VK_FORMAT_R8G8B8A8_SNORM:
            result = 4;
            break;
        case VK_FORMAT_R8G8B8A8_USCALED:
            result = 4;
            break;
        case VK_FORMAT_R8G8B8A8_SSCALED:
            result = 4;
            break;
        case VK_FORMAT_R8G8B8A8_UINT:
            result = 4;
            break;
        case VK_FORMAT_R8G8B8A8_SINT:
            result = 4;
            break;
        case VK_FORMAT_R8G8B8A8_SRGB:
            result = 4;
            break;
        case VK_FORMAT_B8G8R8A8_UNORM:
            result = 4;
            break;
        case VK_FORMAT_B8G8R8A8_SNORM:
            result = 4;
            break;
        case VK_FORMAT_B8G8R8A8_USCALED:
            result = 4;
            break;
        case VK_FORMAT_B8G8R8A8_SSCALED:
            result = 4;
            break;
        case VK_FORMAT_B8G8R8A8_UINT:
            result = 4;
            break;
        case VK_FORMAT_B8G8R8A8_SINT:
            result = 4;
            break;
        case VK_FORMAT_B8G8R8A8_SRGB:
            result = 4;
            break;
        case VK_FORMAT_A8B8G8R8_UNORM_PACK32:
            result = 4;
            break;
        case VK_FORMAT_A8B8G8R8_SNORM_PACK32:
            result = 4;
            break;
        case VK_FORMAT_A8B8G8R8_USCALED_PACK32:
            result = 4;
            break;
        case VK_FORMAT_A8B8G8R8_SSCALED_PACK32:
            result = 4;
            break;
        case VK_FORMAT_A8B8G8R8_UINT_PACK32:
            result = 4;
            break;
        case VK_FORMAT_A8B8G8R8_SINT_PACK32:
            result = 4;
            break;
        case VK_FORMAT_A8B8G8R8_SRGB_PACK32:
            result = 4;
            break;
        case VK_FORMAT_A2R10G10B10_UNORM_PACK32:
            result = 4;
            break;
        case VK_FORMAT_A2R10G10B10_SNORM_PACK32:
            result = 4;
            break;
        case VK_FORMAT_A2R10G10B10_USCALED_PACK32:
            result = 4;
            break;
        case VK_FORMAT_A2R10G10B10_SSCALED_PACK32:
            result = 4;
            break;
        case VK_FORMAT_A2R10G10B10_UINT_PACK32:
            result = 4;
            break;
        case VK_FORMAT_A2R10G10B10_SINT_PACK32:
            result = 4;
            break;
        case VK_FORMAT_A2B10G10R10_UNORM_PACK32:
            result = 4;
            break;
        case VK_FORMAT_A2B10G10R10_SNORM_PACK32:
            result = 4;
            break;
        case VK_FORMAT_A2B10G10R10_USCALED_PACK32:
            result = 4;
            break;
        case VK_FORMAT_A2B10G10R10_SSCALED_PACK32:
            result = 4;
            break;
        case VK_FORMAT_A2B10G10R10_UINT_PACK32:
            result = 4;
            break;
        case VK_FORMAT_A2B10G10R10_SINT_PACK32:
            result = 4;
            break;
        case VK_FORMAT_R16_UNORM:
            result = 2;
            break;
        case VK_FORMAT_R16_SNORM:
            result = 2;
            break;
        case VK_FORMAT_R16_USCALED:
            result = 2;
            break;
        case VK_FORMAT_R16_SSCALED:
            result = 2;
            break;
        case VK_FORMAT_R16_UINT:
            result = 2;
            break;
        case VK_FORMAT_R16_SINT:
            result = 2;
            break;
        case VK_FORMAT_R16_SFLOAT:
            result = 2;
            break;
        case VK_FORMAT_R16G16_UNORM:
            result = 4;
            break;
        case VK_FORMAT_R16G16_SNORM:
            result = 4;
            break;
        case VK_FORMAT_R16G16_USCALED:
            result = 4;
            break;
        case VK_FORMAT_R16G16_SSCALED:
            result = 4;
            break;
        case VK_FORMAT_R16G16_UINT:
            result = 4;
            break;
        case VK_FORMAT_R16G16_SINT:
            result = 4;
            break;
        case VK_FORMAT_R16G16_SFLOAT:
            result = 4;
            break;
        case VK_FORMAT_R16G16B16_UNORM:
            result = 6;
            break;
        case VK_FORMAT_R16G16B16_SNORM:
            result = 6;
            break;
        case VK_FORMAT_R16G16B16_USCALED:
            result = 6;
            break;
        case VK_FORMAT_R16G16B16_SSCALED:
            result = 6;
            break;
        case VK_FORMAT_R16G16B16_UINT:
            result = 6;
            break;
        case VK_FORMAT_R16G16B16_SINT:
            result = 6;
            break;
        case VK_FORMAT_R16G16B16_SFLOAT:
            result = 6;
            break;
        case VK_FORMAT_R16G16B16A16_UNORM:
            result = 8;
            break;
        case VK_FORMAT_R16G16B16A16_SNORM:
            result = 8;
            break;
        case VK_FORMAT_R16G16B16A16_USCALED:
            result = 8;
            break;
        case VK_FORMAT_R16G16B16A16_SSCALED:
            result = 8;
            break;
        case VK_FORMAT_R16G16B16A16_UINT:
            result = 8;
            break;
        case VK_FORMAT_R16G16B16A16_SINT:
            result = 8;
            break;
        case VK_FORMAT_R16G16B16A16_SFLOAT:
            result = 8;
            break;
        case VK_FORMAT_R32_UINT:
            result = 4;
            break;
        case VK_FORMAT_R32_SINT:
            result = 4;
            break;
        case VK_FORMAT_R32_SFLOAT:
            result = 4;
            break;
        case VK_FORMAT_R32G32_UINT:
            result = 8;
            break;
        case VK_FORMAT_R32G32_SINT:
            result = 8;
            break;
        case VK_FORMAT_R32G32_SFLOAT:
            result = 8;
            break;
        case VK_FORMAT_R32G32B32_UINT:
            result = 12;
            break;
        case VK_FORMAT_R32G32B32_SINT:
            result = 12;
            break;
        case VK_FORMAT_R32G32B32_SFLOAT:
            result = 12;
            break;
        case VK_FORMAT_R32G32B32A32_UINT:
            result = 16;
            break;
        case VK_FORMAT_R32G32B32A32_SINT:
            result = 16;
            break;
        case VK_FORMAT_R32G32B32A32_SFLOAT:
            result = 16;
            break;
        case VK_FORMAT_R64_UINT:
            result = 8;
            break;
        case VK_FORMAT_R64_SINT:
            result = 8;
            break;
        case VK_FORMAT_R64_SFLOAT:
            result = 8;
            break;
        case VK_FORMAT_R64G64_UINT:
            result = 16;
            break;
        case VK_FORMAT_R64G64_SINT:
            result = 16;
            break;
        case VK_FORMAT_R64G64_SFLOAT:
            result = 16;
            break;
        case VK_FORMAT_R64G64B64_UINT:
            result = 24;
            break;
        case VK_FORMAT_R64G64B64_SINT:
            result = 24;
            break;
        case VK_FORMAT_R64G64B64_SFLOAT:
            result = 24;
            break;
        case VK_FORMAT_R64G64B64A64_UINT:
            result = 32;
            break;
        case VK_FORMAT_R64G64B64A64_SINT:
            result = 32;
            break;
        case VK_FORMAT_R64G64B64A64_SFLOAT:
            result = 32;
            break;
        case VK_FORMAT_B10G11R11_UFLOAT_PACK32:
            result = 4;
            break;
        case VK_FORMAT_E5B9G9R9_UFLOAT_PACK32:
            result = 4;
            break;

        default:
            break;
        }
        return result;
    }

    Device::Device()
    {
        for (auto&& frame_resource : m_frame_resource) {
            frame_resource = std::make_unique<Frame_resource>(*this);
        }

        m_resource_manager = std::make_unique<Resource_manager>(*this);
        m_shader_manager   = std::make_unique<Shader_manager>(*this);
    }

    void Device::create(HINSTANCE hinstance, HWND hwnd)
    {
        m_hinstance = hinstance;
        m_hwnd      = hwnd;

        try {
            //create_from_initialization_helper(); // retired code
            create_from_vk_bootstrap();

            create_vma_allocator();
            create_command_buffer();

            create_descriptor_pool();
            create_depth_buffer();

            create_sync_object();
        }
        catch (vk::SystemError& err) {
            DBG::OutputString("vk::SystemError: %s\n", err.what());
            exit(-1);
        }
        catch (std::exception& err) {
            DBG::OutputString("std::exception: %s\n", err.what());
            exit(-1);
        }
        catch (...) {
            DBG::OutputString("unknown error\n");
            exit(-1);
        }
    }

    void Device::create_from_initialization_helper()
    {
        Retired_code::Initialization_helper helper;

        // create Instance
        helper.m_req_api_version          = m_req_api_version;
        helper.m_instance_extension_names = get_instance_extensions();

        helper.create_instance();

        m_vk_instance = helper.m_vk_instance;

        // create Surface - and provide it to our helper
        create_surface();

        // create Device
        helper.m_surface                         = m_surface;
        helper.m_physical_device_extension_names = get_device_extensions();

        helper.create_device();

        m_physical_device = helper.m_physical_device;
        m_device          = helper.m_device;

        // create Swapchain
        auto&& win_rect = get_window_rect();

        helper.create_swapchain(win_rect);

        m_swapchain             = helper.m_swapchain;
        m_swapchain_image_views = helper.m_swapchain_image_views;
        m_swapchain_images      = helper.m_swapchain_images;
        m_swapchain_image_size  = helper.m_swapchain_image_size;
        m_swapchain_format      = helper.m_swapchain_format;

        // get queues
        m_graphics_queue_family_index = helper.m_graphics_queue_family_index;
        m_present_queue_family_index = helper.m_present_queue_family_index;
        m_graphics_queue = helper.m_graphics_queue;
        m_present_queue = helper.m_present_queue;
    }

    void Device::create_from_vk_bootstrap()
    {
#if (VULKAN_HPP_DISPATCH_LOADER_DYNAMIC == 1)
        static vk::detail::DynamicLoader dl;
        PFN_vkGetInstanceProcAddr        vkGetInstanceProcAddr =
            dl.getProcAddress<PFN_vkGetInstanceProcAddr>("vkGetInstanceProcAddr");
        VULKAN_HPP_DEFAULT_DISPATCHER.init(vkGetInstanceProcAddr);
#endif

        // create vk instance
        vkb::InstanceBuilder instance_builder;

        auto&& instance_extensions = get_instance_extensions();

        auto&& ret_instance = instance_builder.require_api_version(m_req_api_version)
                                  .enable_extensions(instance_extensions)
                                  .request_validation_layers()
                                  .use_default_debug_messenger()
                                  .build();

        if (!ret_instance) {
            throw std::runtime_error("Failed to create instance");
        }

        auto&& vkb_instance = ret_instance.value();
        m_vk_instance       = vk::Instance(vkb_instance.instance);

#if (VULKAN_HPP_DISPATCH_LOADER_DYNAMIC == 1)
        // initialize function pointers for instance
        VULKAN_HPP_DEFAULT_DISPATCHER.init(m_vk_instance);
#endif

        // create win32 surface
        create_surface();

        // create physical device and logical device
        auto&& device_extensions = get_device_extensions();

        auto&& required_dynamic_rendering = vk::PhysicalDeviceDynamicRenderingFeaturesKHR{
            .dynamicRendering = VK_TRUE,
        };

        auto&& required_synchronization2 = vk::PhysicalDeviceSynchronization2FeaturesKHR{
            .synchronization2 = VK_TRUE,
        };

        vkb::PhysicalDeviceSelector selector{vkb_instance};
        auto&&                      ret_physical_device = selector.set_surface(static_cast<VkSurfaceKHR>(m_surface))
                                         .add_required_extensions(device_extensions)
                                         .add_required_extension_features(required_dynamic_rendering)
                                         .add_required_extension_features(required_synchronization2)
                                         .select();

        if (!ret_physical_device) {
            throw std::runtime_error("Failed to select physical device");
        }

        auto&& vkb_physical_device = ret_physical_device.value();
        m_physical_device          = vk::PhysicalDevice(vkb_physical_device);

        vkb::DeviceBuilder device_builder{vkb_physical_device};

        auto&& ret_device = device_builder.build();

        if (!ret_device) {
            throw std::runtime_error("Failed to create logical device");
        }

        auto&& vkb_device = ret_device.value();
        m_device          = vk::Device(vkb_device);

#if (VULKAN_HPP_DISPATCH_LOADER_DYNAMIC == 1)
        // initialize function pointers for instance
        VULKAN_HPP_DEFAULT_DISPATCHER.init(m_device);
#endif

        // create swapchain
        vkb::SwapchainBuilder swapchain_builder{vkb_device};

        auto&& ret_swapchain = swapchain_builder.set_old_swapchain(m_vkb_swapchain).build();

        if (!ret_swapchain) {
            throw std::runtime_error(ret_swapchain.error().message());
        }

        vkb::destroy_swapchain(m_vkb_swapchain);
        m_vkb_swapchain = ret_swapchain.value();

        m_swapchain            = m_vkb_swapchain.swapchain;
        m_swapchain_image_size = m_vkb_swapchain.extent;

        auto&& vkb_swapchain_images = m_vkb_swapchain.get_images().value();
        std::copy(vkb_swapchain_images.begin(), vkb_swapchain_images.end(), std::back_inserter(m_swapchain_images));

        auto&& vkb_swapchain_image_views = m_vkb_swapchain.get_image_views().value();
        std::copy(
            vkb_swapchain_image_views.begin(), vkb_swapchain_image_views.end(), std::back_inserter(m_swapchain_image_views));

        m_swapchain_format = vk::Format(m_vkb_swapchain.image_format);

        // get queues
        auto&& ret_graphics_queue = vkb_device.get_queue(vkb::QueueType::graphics);
        if (!ret_graphics_queue.has_value()) {
            auto&& err_string = DBG::Format("failed to get graphics queue: %s\n", ret_graphics_queue.error().message());
            throw std::runtime_error(err_string);
        }
        m_graphics_queue = vk::Queue(ret_graphics_queue.value());

        auto ret_present_queue = vkb_device.get_queue(vkb::QueueType::present);
        if (!ret_present_queue.has_value()) {
            auto&& err_string = DBG::Format("failed to get present queue: %s\n", ret_present_queue.error().message());
            throw std::runtime_error(err_string);
        }
        m_present_queue = vk::Queue(ret_present_queue.value());
    }

    void Device::create_surface()
    {
        // create Surface from Win32;
        vk::Win32SurfaceCreateInfoKHR win32_surface_createinfo{
            .flags     = vk::Win32SurfaceCreateFlagsKHR(),
            .hinstance = m_hinstance,
            .hwnd      = m_hwnd,
        };

        m_surface = m_vk_instance.createWin32SurfaceKHR(win32_surface_createinfo);
    }

    void Device::destroy()
    {
        // wait until GPU finished all the work
        m_device.waitIdle();

        // destroy sync object
        destroy_sync_object();

        // retired - destroy render pass
        // for (auto framebuffer : m_frame_buffers) {
        //    m_device.destroyFramebuffer(framebuffer);
        //}
        // m_device.destroyRenderPass(m_render_pass);

        // destroy shader
        m_shader_manager->destroy_resources();

        // destroy resources
        m_resource_manager->destroy();
        destroy_resource(m_depth_buffer);

        // destroy the imageViews, the swapChain,and the surface
        for (auto& view : m_swapchain_image_views) {
            m_device.destroyImageView(view);
        }
        m_device.destroySwapchainKHR(m_swapchain);
        m_vk_instance.destroySurfaceKHR(m_surface);

        // destroy command buffer
        // freeing the commandBuffer is optional, as it will automatically freed when the
        // corresponding CommandPool is destroyed.
        for (uint32_t i = 0; i < m_frame_resource.size(); ++i) {
            m_device.freeCommandBuffers(m_command_pool, m_frame_resource[i]->m_command_buffer);
            m_frame_resource[i]->destroy_resources();
        }
        // destroy the command pool
        m_device.destroyCommandPool(m_command_pool);

        // vma allocator
        m_vma_allocator.destroy();

        // destroy the device
        m_device.destroy();

        // destroy instance
        // m_vk_instance.destroyDebugUtilsMessengerEXT(m_debug_utils_messenger); // retired : move to vk-bootstrap
        m_vk_instance.destroy();
    }

    void Device::create_vma_allocator()
    {
        vma::AllocatorCreateInfo createinfo;
        // set allocator properties
        createinfo.physicalDevice   = m_physical_device;
        createinfo.device           = m_device;
        createinfo.instance         = m_vk_instance;
        createinfo.vulkanApiVersion = m_req_api_version;

        m_vma_allocator = vma::createAllocator(createinfo);
    }

    void Device::create_command_buffer()
    {
        // create a CommandPool to allocate a CommandBuffer from
        vk::CommandPoolCreateInfo command_pool_info{
            .flags            = vk::CommandPoolCreateFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer),
            .queueFamilyIndex = m_graphics_queue_family_index};
        m_command_pool = m_device.createCommandPool(command_pool_info);

        // allocate a CommandBuffer from the CommandPool
        vk::CommandBufferAllocateInfo commmand_buffer_allocate_info{
            .commandPool        = m_command_pool,
            .level              = vk::CommandBufferLevel::ePrimary,
            .commandBufferCount = MAX_FRAMES_IN_FLIGHT,
        };
        auto&& command_buffers = m_device.allocateCommandBuffers(commmand_buffer_allocate_info);

        for (uint32_t i = 0; i < m_frame_resource.size(); ++i) {
            m_frame_resource[i]->m_command_buffer = command_buffers[i];
        }
    }

    void Device::create_depth_buffer()
    {
        const vk::Format     depth_format      = vk::Format::eD24UnormS8Uint;
        vk::FormatProperties format_properties = m_physical_device.getFormatProperties(depth_format);

        vk::ImageTiling tiling;
        if (format_properties.linearTilingFeatures & vk::FormatFeatureFlagBits::eDepthStencilAttachment) {
            tiling = vk::ImageTiling::eLinear;
        }
        else if (format_properties.optimalTilingFeatures & vk::FormatFeatureFlagBits::eDepthStencilAttachment) {
            tiling = vk::ImageTiling::eOptimal;
        }
        else {
            throw std::runtime_error("DepthStencilAttachment is not supported for D16Unorm depth format.");
        }

        auto&& r = get_window_rect();

        vk::ImageCreateInfo image_createinfo{
            .imageType   = vk::ImageType::e2D,
            .format      = depth_format,
            .extent      = vk::Extent3D(r.Width(), r.Height(), 1),
            .mipLevels   = 1,
            .arrayLayers = 1,
            .samples     = vk::SampleCountFlagBits::e1,
            .tiling      = tiling,
            .usage       = vk::ImageUsageFlagBits::eDepthStencilAttachment,
        };

        vma::AllocationCreateInfo alloc_createinfo;
        alloc_createinfo.usage = vma::MemoryUsage::eAuto;

        m_depth_buffer.m_format = depth_format;

        std::tie(m_depth_buffer.m_image, m_depth_buffer.m_alloc) =
            m_vma_allocator.createImage(image_createinfo, alloc_createinfo);

        // create image views
        vk::ImageViewCreateInfo image_view_createinfo{
            .flags    = vk::ImageViewCreateFlags(),
            .image    = m_depth_buffer.m_image,
            .viewType = vk::ImageViewType::e2D,
            .format   = depth_format,
            .subresourceRange =
                vk::ImageSubresourceRange{
                    .aspectMask     = vk::ImageAspectFlagBits::eDepth,
                    .baseMipLevel   = 0,
                    .levelCount     = 1,
                    .baseArrayLayer = 0,
                    .layerCount     = 1,
                },
        };

        m_depth_buffer.m_view = m_device.createImageView(image_view_createinfo);
    }

    /*
    void Device::create_render_pass()
    {
            vk::Format color_format =
    pick_surface_format(m_physical_device.getSurfaceFormatsKHR(m_surface)).format; auto&&
    depth_format = m_depth_buffer.m_format;

            // Render pass ----------
            std::array<vk::AttachmentDescription, 2> attachment_desc;
            attachment_desc[0] = vk::AttachmentDescription(vk::AttachmentDescriptionFlags(),
    color_format, vk::SampleCountFlagBits::e1, vk::AttachmentLoadOp::eClear,
    vk::AttachmentStoreOp::eStore, vk::AttachmentLoadOp::eDontCare,
    vk::AttachmentStoreOp::eDontCare, vk::ImageLayout::eUndefined, vk::ImageLayout::ePresentSrcKHR);
            attachment_desc[1] = vk::AttachmentDescription(vk::AttachmentDescriptionFlags(),
    depth_format, vk::SampleCountFlagBits::e1, vk::AttachmentLoadOp::eClear,
    vk::AttachmentStoreOp::eDontCare, vk::AttachmentLoadOp::eDontCare,
    vk::AttachmentStoreOp::eDontCare, vk::ImageLayout::eUndefined,
    vk::ImageLayout::eDepthStencilAttachmentOptimal);

            vk::AttachmentReference color_reference(0, vk::ImageLayout::eColorAttachmentOptimal);
            vk::AttachmentReference depth_reference(1,
    vk::ImageLayout::eDepthStencilAttachmentOptimal); vk::SubpassDescription
    subpass(vk::SubpassDescriptionFlags(), vk::PipelineBindPoint::eGraphics, {}, color_reference,
    {}, &depth_reference);

            m_render_pass =
    m_device.createRenderPass(vk::RenderPassCreateInfo(vk::RenderPassCreateFlags(), attachment_desc,
    subpass));
            // ----------

            // Note: if multiple RT is used, attachments should equal numRT+Depth
            // Frame buffer for Back buffers
            vk::ImageView attachments[2];
            attachments[1] = m_depth_buffer.m_view;

            auto&& r = get_window_rect();

            vk::FramebufferCreateInfo framebuffer_createinfo(
                    vk::FramebufferCreateFlags(), m_render_pass, m_depth_buffer.m_view ? 2 : 1,
    attachments, r.Width(), r.Height(), 1);

            m_frame_buffers.reserve(m_swapchain_image_views.size());
            for (auto&& view : m_swapchain_image_views) {
                    attachments[0] = view;
                    m_frame_buffers.push_back(m_device.createFramebuffer(framebuffer_createinfo));
            }
    }
    */

    void Device::create_sync_object()
    {
        for (uint32_t i = 0; i < m_frame_resource.size(); ++i) {

            m_frame_resource[i]->m_image_available_semaphore = m_device.createSemaphore(vk::SemaphoreCreateInfo());
            m_frame_resource[i]->m_render_finished_semaphore = m_device.createSemaphore(vk::SemaphoreCreateInfo());
            // initialize with the Signaled state
            vk::FenceCreateInfo fence_createinfo{
                .flags = vk::FenceCreateFlagBits::eSignaled,
            };

            m_frame_resource[i]->m_inflight_fence = m_device.createFence(fence_createinfo);
        }
    }

    void Device::destroy_sync_object()
    {
        for (uint32_t i = 0; i < m_frame_resource.size(); ++i) {
            m_device.destroyFence(m_frame_resource[i]->m_inflight_fence);
            m_device.destroySemaphore(m_frame_resource[i]->m_render_finished_semaphore);
            m_device.destroySemaphore(m_frame_resource[i]->m_image_available_semaphore);
        }
    }

    void Device::create_descriptor_pool()
    {
        for (auto&& frame_resource : m_frame_resource) {
            frame_resource->m_descriptor_pool.create_pool();
        }
    }

    // void Device::draw_once()
    //{
    //     auto&& image_available_semaphore = m_frame_resource[0]->m_image_available_semaphore;
    //     auto&& inflight_fence            = m_frame_resource[0]->m_inflight_fence;
    //
    //     auto&& command_buffer = m_frame_resource[0]->m_command_buffer;
    //
    //     // Get the index of the next available swapchain image:
    //     vk::ResultValue<uint32_t> current_buffer = m_device.acquireNextImageKHR(m_swapchain,
    //     m_fence_timeout, image_available_semaphore, nullptr); assert(current_buffer.result ==
    //     vk::Result::eSuccess); assert(current_buffer.value < m_frame_buffers.size());
    //
    //     command_buffer.begin(vk::CommandBufferBeginInfo(vk::CommandBufferUsageFlags()));
    //
    //     std::array<vk::ClearValue, 2> clear_values;
    //     clear_values[0].color        = vk::ClearColorValue(std::array<float, 4>({{0.2f, 0.2f,
    //     0.2f, 0.2f}})); clear_values[1].depthStencil = vk::ClearDepthStencilValue(1.0f, 0);
    //
    //     auto&& r        = get_window_rect();
    //     auto&& extent2d = vk::Extent2D(r.Width(), r.Height());
    //
    //     vk::RenderPassBeginInfo renderPassBeginInfo(
    //         m_render_pass, m_frame_buffers[current_buffer.value], vk::Rect2D(vk::Offset2D(0, 0),
    //         extent2d), clear_values);
    //     command_buffer.beginRenderPass(renderPassBeginInfo, vk::SubpassContents::eInline);
    //     command_buffer.bindPipeline(vk::PipelineBindPoint::eGraphics, m_pipeline);
    //     // m_command_buffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, pipelineLayout,
    //     0, descriptorSet, nullptr);
    //
    //     // m_command_buffer.bindVertexBuffers(0, vertexBufferData.buffer, {0});
    //     command_buffer.setViewport(0, vk::Viewport(0.0f, 0.0f,
    //     static_cast<float>(extent2d.width), static_cast<float>(extent2d.height), 0.0f, 1.0f));
    //     command_buffer.setScissor(0, vk::Rect2D(vk::Offset2D(0, 0), extent2d));
    //
    //     // m_command_buffer.draw(12 * 3, 1, 0, 0);
    //     command_buffer.draw(3, 1, 0, 0);
    //     command_buffer.endRenderPass();
    //     command_buffer.end();
    //
    //     vk::PipelineStageFlags
    //     wait_destination_stage_mask(vk::PipelineStageFlagBits::eColorAttachmentOutput);
    //     vk::SubmitInfo         submit_info(image_available_semaphore,
    //     wait_destination_stage_mask, command_buffer);
    //
    //     vk::Queue graphics_queue = m_device.getQueue(m_graphics_queue_family_index, 0);
    //     graphics_queue.submit(submit_info, inflight_fence);
    //
    //     while (vk::Result::eTimeout == m_device.waitForFences(inflight_fence, VK_TRUE,
    //     m_fence_timeout)) {
    //     };
    //
    //     vk::Queue  present_queue = m_device.getQueue(m_present_queue_family_index, 0);
    //     vk::Result result        = present_queue.presentKHR(vk::PresentInfoKHR({}, m_swapchain,
    //     current_buffer.value)); switch (result) { case vk::Result::eSuccess:
    //         break;
    //     case vk::Result::eSuboptimalKHR:
    //         std::cout << "vk::Queue::presentKHR returned vk::Result::eSuboptimalKHR !\n";
    //         break;
    //     default:
    //         assert(false); // an unexpected result is returned !
    //     }
    //     std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    //
    //     m_device.waitIdle();
    // }

    void Device::draw()
    {
        begin_frame();

        auto&& command_buffer = curr_command_buffer();
        if (!command_buffer) {
            return;
        }

        // std::array<vk::ClearValue, 2> clear_values;
        // clear_values[0].color		 = vk::ClearColorValue(std::array<float, 4>({{0.2f,
        // 0.2f, 0.2f, 0.2f}})); clear_values[1].depthStencil = vk::ClearDepthStencilValue(1.0f, 0);
        // vk::RenderPassBeginInfo renderPassBeginInfo(
        //	m_render_pass, m_frame_buffers[m_swapchain_buffer_idx], vk::Rect2D(vk::Offset2D(0,
        // 0), m_swapchain_image_size), clear_values);
        // command_buffer->beginRenderPass(renderPassBeginInfo, vk::SubpassContents::eInline);

        // setup render pass
        auto&& render_target_image = get_backbuffer_colour_image();
        auto&& depth_target_image  = m_depth_buffer.m_image;

        transition_image_layout(render_target_image,
            Transition_image_layout_info{
                .dst_layout       = vk::ImageLayout::eColorAttachmentOptimal,
                .src_layout       = vk::ImageLayout::eUndefined,
                .dst_access_flags = vk::AccessFlagBits2::eColorAttachmentWrite,
                .src_access_flags = vk::AccessFlagBits2::eNone,
                .dst_stage_flags  = vk::PipelineStageFlagBits2::eColorAttachmentOutput,
                .src_stage_flags  = vk::PipelineStageFlagBits2::eTopOfPipe,
            });

        vk::ClearColorValue        clear_colour{{{0.2f, 0.2f, 0.2f, 0.2f}}};
        vk::ClearDepthStencilValue clear_depth = {
            .depth   = 1.0f,
            .stencil = 0u,
        };

        vk::RenderingAttachmentInfo colour_attachment{
            .imageView   = get_backbuffer_colour_image_view(),
            .imageLayout = vk::ImageLayout::eColorAttachmentOptimal,
            .loadOp      = vk::AttachmentLoadOp::eClear,
            .storeOp     = vk::AttachmentStoreOp::eStore,
            .clearValue  = clear_colour,
        };

        vk::RenderingInfo rendering_info{
            .renderArea =
                {
                    .offset = {0, 0},
                    .extent = m_swapchain_image_size,
                },
            .layerCount           = 1,
            .colorAttachmentCount = 1,
            .pColorAttachments    = &colour_attachment,

        };

        command_buffer->beginRendering(&rendering_info);

        command_buffer->setViewport(0,
            vk::Viewport(0.0f,
                0.0f,
                static_cast<float>(m_swapchain_image_size.width),
                static_cast<float>(m_swapchain_image_size.height),
                0.0f,
                1.0f));
        command_buffer->setScissor(0, vk::Rect2D(vk::Offset2D(0, 0), m_swapchain_image_size));

        auto&& t0 = m_shader_manager->get_technique("t0").lock();
        command_buffer->bindPipeline(vk::PipelineBindPoint::eGraphics, t0->m_pipeline);
        // m_command_buffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, pipelineLayout, 0,
        // descriptorSet, nullptr);

        // command_buffer.bindVertexBuffers(0, 0, nullptr, nullptr);

        // m_command_buffer.draw(12 * 3, 1, 0, 0);
        command_buffer->draw(3, 1, 0, 0);

        // 2nd draw
        auto&& technique          = m_shader_manager->get_technique("t1").lock();
        auto&& technique_instance = Technique_instance(*technique);
        float  data[]             = {4.0f, 1.0f};
        technique_instance.set_constant("Data_cbv", data, sizeof(data));

        command_buffer->bindPipeline(vk::PipelineBindPoint::eGraphics, technique->m_pipeline);
        technique_instance.set_descriptor_set_parameters();

        command_buffer->bindVertexBuffers(0, m_resource_manager->m_vertex_buffer.m_buffer, {0});
        command_buffer->bindIndexBuffer(m_resource_manager->m_index_buffer.m_buffer, 0, vk::IndexType::eUint32);

        command_buffer->drawIndexed(36, 1, 0, 0, 0);

        // command_buffer->endRenderPass();

        command_buffer->endRendering();

        transition_image_layout(render_target_image,
            Transition_image_layout_info{
                .dst_layout       = vk::ImageLayout::ePresentSrcKHR,
                .src_layout       = vk::ImageLayout::eColorAttachmentOptimal,
                .dst_access_flags = vk::AccessFlagBits2::eNone,
                .src_access_flags = vk::AccessFlagBits2::eColorAttachmentWrite,
                .dst_stage_flags  = vk::PipelineStageFlagBits2::eBottomOfPipe,
                .src_stage_flags  = vk::PipelineStageFlagBits2::eColorAttachmentOutput,
            });

        end_frame();
    }

    void Device::load_resources()
    {
        // use dynamic rendering
        // create_render_pass();
    }

    void Device::begin_single_command_submission()
    {
        auto&& alloc_info = vk::CommandBufferAllocateInfo{
            .commandPool        = m_command_pool,
            .level              = vk::CommandBufferLevel::ePrimary,
            .commandBufferCount = 1,
        };

        auto&& result = m_device.allocateCommandBuffers(&alloc_info, &m_single_use_command_buffer);

        auto&& begin_info = vk::CommandBufferBeginInfo{.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit};
        m_single_use_command_buffer.begin(begin_info);
    }

    void Device::end_single_command_submission()
    {
        m_single_use_command_buffer.end();

        auto&& submit_info = vk::SubmitInfo{
            .commandBufferCount = 1,
            .pCommandBuffers    = &m_single_use_command_buffer,
        };

        vk::Queue graphics_queue = m_device.getQueue(m_graphics_queue_family_index, 0);
        graphics_queue.submit(submit_info);

        graphics_queue.waitIdle();

        m_device.freeCommandBuffers(m_command_pool, m_single_use_command_buffer);
    }

    void Device::begin_frame()
    {
        auto&& frame_resource_idx = curr_frame_resource_idx();
        auto&& frame_resource     = m_frame_resource[frame_resource_idx];

        auto&& image_available_semaphore = frame_resource->m_image_available_semaphore;
        auto&& render_finished_semaphore = frame_resource->m_render_finished_semaphore;
        auto&& inflight_fence            = frame_resource->m_inflight_fence;

        // UINT64_MAX, which effectively disables the timeout.
        auto&& result = m_device.waitForFences(inflight_fence, VK_TRUE, UINT64_MAX);
        m_device.resetFences(inflight_fence);

        // workaround for eErrorOutOfDateKHR
        // https://github.com/KhronosGroup/Vulkan-Hpp/issues/599
        result = m_device.acquireNextImageKHR(
            m_swapchain, m_fence_timeout, image_available_semaphore, nullptr, &m_swapchain_buffer_idx);
        if (result == vk::Result::eErrorOutOfDateKHR) {
            auto&& command_buffer = m_frame_resource[frame_resource_idx]->m_command_buffer;
            command_buffer.reset();
            // recreate swapchain
            return;
        }
        else if (result != vk::Result::eSuccess) {
            throw std::runtime_error("failed to acquire swap chain image!");
        }

        // assert(m_swapchain_buffer_idx < m_frame_buffers.size());

        // destroy per flight resources
        frame_resource->begin_frame();
    }

    void Device::end_frame()
    {
        auto&& frame_resource_idx = curr_frame_resource_idx();

        auto&& image_available_semaphore = m_frame_resource[frame_resource_idx]->m_image_available_semaphore;
        auto&& render_finished_semaphore = m_frame_resource[frame_resource_idx]->m_render_finished_semaphore;
        auto&& inflight_fence            = m_frame_resource[frame_resource_idx]->m_inflight_fence;

        auto&& command_buffer = m_frame_resource[frame_resource_idx]->m_command_buffer;

        m_frame_resource[frame_resource_idx]->end_frame();

        // submit the queue
        vk::PipelineStageFlags wait_destination_stage_mask(vk::PipelineStageFlagBits::eColorAttachmentOutput);

        vk::SubmitInfo submit_info{
            .waitSemaphoreCount   = 1,
            .pWaitSemaphores      = &image_available_semaphore,
            .pWaitDstStageMask    = &wait_destination_stage_mask,
            .commandBufferCount   = 1,
            .pCommandBuffers      = &command_buffer,
            .signalSemaphoreCount = 1,
            .pSignalSemaphores    = &render_finished_semaphore,
        };

        vk::Queue graphics_queue = m_device.getQueue(m_graphics_queue_family_index, 0);
        graphics_queue.submit(submit_info, inflight_fence);

        vk::Queue present_queue = m_device.getQueue(m_present_queue_family_index, 0);

        // workaround for eErrorOutOfDateKHR throw exception when Quiting
        // https://github.com/KhronosGroup/Vulkan-Hpp/issues/599
        auto&& present_info = vk::PresentInfoKHR{
            .waitSemaphoreCount = 1,
            .pWaitSemaphores    = &render_finished_semaphore,
            .swapchainCount     = 1,
            .pSwapchains        = &m_swapchain,
            .pImageIndices      = &m_swapchain_buffer_idx,
        };
        auto&& present_result = present_queue.presentKHR(&present_info);

        switch (present_result) {
        case vk::Result::eSuccess:
            break;
        case vk::Result::eErrorOutOfDateKHR:
            std::cout << "vk::Queue::presentKHR returned vk::Result::eErrorOutOfDateKHR !\n";
            break;
        case vk::Result::eSuboptimalKHR:
            std::cout << "vk::Queue::presentKHR returned vk::Result::eSuboptimalKHR !\n";
            break;
        default:
            assert(false); // an unexpected result is returned !
        }
    }

    vk::Format Device::get_backbuffer_colour_format() const
    {
        return m_swapchain_format;
        // return pick_surface_format(m_physical_device.getSurfaceFormatsKHR(m_surface)).format;
    }

    vk::Format Device::get_backbuffer_depth_format() const { return m_depth_buffer.m_format; }

    void Device::transition_image_layout(vk::Image image, const Transition_image_layout_info& transition_image_layout_info)
    {
        vk::ImageMemoryBarrier2 image_barrier{// Specify the pipeline stages and access masks for the barrier
            .srcStageMask  = transition_image_layout_info.src_stage_flags,  // Source pipeline stage mask
            .srcAccessMask = transition_image_layout_info.src_access_flags, // Source access mask
            .dstStageMask  = transition_image_layout_info.dst_stage_flags,  // Destination pipeline stage mask
            .dstAccessMask = transition_image_layout_info.dst_access_flags, // Destination access mask

            // Specify the old and new layouts of the image
            .oldLayout = transition_image_layout_info.src_layout, // Current layout of the image
            .newLayout = transition_image_layout_info.dst_layout, // Target layout of the image

            // We are not changing the ownership between queues
            .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,

            // Specify the image to be affected by this barrier
            .image = image,

            // Define the subresource range (which parts of the image are affected)
            .subresourceRange = {
                .aspectMask     = vk::ImageAspectFlagBits::eColor, // Affects the color aspect of the image
                .baseMipLevel   = 0,                               // Start at mip level 0
                .levelCount     = 1,                               // Number of mip levels affected
                .baseArrayLayer = 0,                               // Start at array layer 0
                .layerCount     = 1                                // Number of array layers affected
            }};

        vk::DependencyInfo dependency_info{
            .dependencyFlags         = vk::DependencyFlags(), // No special dependency flags
            .imageMemoryBarrierCount = 1,
            .pImageMemoryBarriers    = &image_barrier,
        };

        auto&& frame_resource_idx = curr_frame_resource_idx();
        auto&& command_buffer     = m_frame_resource[frame_resource_idx]->m_command_buffer;

        command_buffer.pipelineBarrier2(&dependency_info);
    }

    std::vector<const char*> Device::get_instance_extensions()
    {
        std::vector<const char*> extensions = {VK_KHR_SURFACE_EXTENSION_NAME
#if defined(VK_USE_PLATFORM_WIN32_KHR)
            ,
            VK_KHR_WIN32_SURFACE_EXTENSION_NAME
#endif
            ,
            VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME};
        return extensions;
    }

    std::vector<const char*> Device::get_device_extensions()
    {
        return {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME,
            VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME,
            VK_KHR_SYNCHRONIZATION_2_EXTENSION_NAME,
        };
    }

    uint32_t Device::curr_frame_resource_idx() const { return m_frame_count % MAX_FRAMES_IN_FLIGHT; }

    Frame_resource& Device::curr_frame_resource()
    {
        auto&& frame_resource_idx = curr_frame_resource_idx();
        return *m_frame_resource[frame_resource_idx];
    }

    vk::CommandBuffer* Device::curr_command_buffer()
    {
        // note: need to address when we are recording using m_single_command as well
        auto&& frame_resource_idx = curr_frame_resource_idx();

        return m_frame_resource[frame_resource_idx]->m_command_buffer_opened
                   ? &m_frame_resource[frame_resource_idx]->m_command_buffer
                   : nullptr;
    }

    void Device::destroy_resource(Image& resource)
    {
        if (resource.m_view) {
            m_device.destroyImageView(resource.m_view);
        }

        if (resource.m_alloc) {
            m_vma_allocator.freeMemory(resource.m_alloc);
        }

        if (resource.m_image) {
            m_device.destroyImage(resource.m_image);
        }
    }

    CRect Device::get_window_rect() const
    {
        CRect client_rect;
        GetClientRect(m_hwnd, &client_rect);
        return client_rect;
    }

    vk::SurfaceFormatKHR Device::pick_surface_format(std::vector<vk::SurfaceFormatKHR> const& formats) const
    {
        assert(!formats.empty());

        vk::SurfaceFormatKHR picked_format = formats[0];
        if (formats.size() == 1) {
            if (formats[0].format == vk::Format::eUndefined) {
                picked_format.format     = vk::Format::eB8G8R8A8Unorm;
                picked_format.colorSpace = vk::ColorSpaceKHR::eSrgbNonlinear;
            }
        }
        else {
            // request several formats, the first found will be used
            std::array<vk::Format, 4> requested_formats = {
                vk::Format::eB8G8R8A8Unorm, vk::Format::eR8G8B8A8Unorm, vk::Format::eB8G8R8Unorm, vk::Format::eR8G8B8Unorm};
            vk::ColorSpaceKHR requested_color_space = vk::ColorSpaceKHR::eSrgbNonlinear;
            for (size_t i = 0; i < requested_formats.size(); i++) {
                vk::Format requested_format = requested_formats[i];

                auto it = std::find_if(formats.begin(),
                    formats.end(),
                    [requested_format, requested_color_space](vk::SurfaceFormatKHR const& f) {
                        return (f.format == requested_format) && (f.colorSpace == requested_color_space);
                    });
                if (it != formats.end()) {
                    picked_format = *it;
                    break;
                }
            }
        }

        assert(picked_format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear);
        return picked_format;
    }

} // namespace VKN
