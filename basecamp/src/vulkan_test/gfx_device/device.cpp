#include "device.h"

#include <thread>

#include "common/common_cpp.h"
#include "resource_manager.h"
#include "shader_manager.h"
#include "technique_instance.h"

VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE

namespace VKN {

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
            create_instance();
            create_device();
            create_vma_allocator();
            create_command_buffer();

            create_descriptor_pool();

            create_swapchain();
            create_depth_buffer();

            create_sync_object();
        }
        catch (vk::SystemError& err) {
            std::cout << "vk::SystemError: " << err.what() << std::endl;
            exit(-1);
        }
        catch (std::exception& err) {
            std::cout << "std::exception: " << err.what() << std::endl;
            exit(-1);
        }
        catch (...) {
            std::cout << "unknown error\n";
            exit(-1);
        }
    }

    void Device::destroy()
    {
        // wait until GPU finished all the work
        m_device.waitIdle();

        // destroy sync object
        destroy_sync_object();

        // destroy render pass
        for (auto framebuffer : m_frame_buffers) {
            m_device.destroyFramebuffer(framebuffer);
        }
        m_device.destroyRenderPass(m_render_pass);

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
        m_instance->destroySurfaceKHR(m_surface);

        // destroy command buffer
        // freeing the commandBuffer is optional, as it will automatically freed when the corresponding CommandPool is destroyed.
        for (uint32_t i = 0; i < m_frame_resource.size(); ++i) {
            m_device.freeCommandBuffers(m_command_pool, m_frame_resource[i]->m_command_buffer);
            m_frame_resource[i]->destroy_resources();
        }
        // destroy the commandPool
        m_device.destroyCommandPool(m_command_pool);

        // vma allocator
        m_vma_allocator.destroy();

        // destroy the device
        m_device.destroy();

        // destroy
        m_instance->destroyDebugUtilsMessengerEXT(m_debug_utils_messenger);
    }

    void Device::create_instance()
    {
        // init paramaters
        const static auto                     app_name    = std::string("vulkan_gfx");
        const static auto                     api_version = m_req_api_version;
        const static std::vector<std::string> layers      = {};
        const static std::vector<std::string> extensions  = get_instance_extensions();

#if (VULKAN_HPP_DISPATCH_LOADER_DYNAMIC == 1)
        static vk::detail::DynamicLoader dl;
        PFN_vkGetInstanceProcAddr        vkGetInstanceProcAddr = dl.getProcAddress<PFN_vkGetInstanceProcAddr>("vkGetInstanceProcAddr");
        VULKAN_HPP_DEFAULT_DISPATCHER.init(vkGetInstanceProcAddr);
#endif

        vk::ApplicationInfo applicationInfo{
            .pApplicationName = app_name.c_str(), .applicationVersion = 1, .pEngineName = app_name.c_str(), .engineVersion = 1, .apiVersion = api_version};

        std::vector<const char*> enabled_layers     = gather_layers(layers, vk::enumerateInstanceLayerProperties());
        std::vector<const char*> enabled_extensions = gather_extensions(extensions, vk::enumerateInstanceExtensionProperties());

        // create instance
        m_instance =
            vk::createInstanceUnique(make_instance_create_info_chain(applicationInfo, enabled_layers, enabled_extensions).get<vk::InstanceCreateInfo>());
#if (VULKAN_HPP_DISPATCH_LOADER_DYNAMIC == 1)
        // initialize function pointers for instance
        VULKAN_HPP_DEFAULT_DISPATCHER.init(*m_instance);
#endif

        // create debug layer
        m_debug_utils_messenger = create_debug_utils_messenger_EXT(*m_instance);
    }

    void Device::create_device()
    {
        // enumerate the physical devices
        m_physical_device = m_instance->enumeratePhysicalDevices().front();

        // get the QueueFamilyProperties of the first PhysicalDevice
        std::vector<vk::QueueFamilyProperties> queue_family_properties = m_physical_device.getQueueFamilyProperties();

        size_t graphics_queue_family_index = find_graphics_queue_family_index(queue_family_properties);
        assert(graphics_queue_family_index < queue_family_properties.size());
        m_graphics_queue_family_index = static_cast<uint32_t>(graphics_queue_family_index);

        // find device extension
        auto&& device_extensions = get_device_extensions();

        // enable dynamic rendering
        auto& requested_dynamic_rendering            = request_extension_features<vk::PhysicalDeviceDynamicRenderingFeaturesKHR>();
        requested_dynamic_rendering.dynamicRendering = VK_TRUE;

        std::vector<const char*> enabled_extensions;
        enabled_extensions.reserve(device_extensions.size());
        for (auto&& ext : device_extensions) {
            enabled_extensions.push_back(ext.data());
        }

        // create a Device
        float                     queue_priority = 0.0f;
        vk::DeviceQueueCreateInfo device_queue_createinfo{
            .flags = vk::DeviceQueueCreateFlags(), .queueFamilyIndex = m_graphics_queue_family_index, .queueCount = 1, .pQueuePriorities = &queue_priority};

        vk::DeviceCreateInfo device_createinfo{
            .pNext                   = &m_last_requested_extension_feature,
            .queueCreateInfoCount    = 1,
            .pQueueCreateInfos       = &device_queue_createinfo,
            .enabledExtensionCount   = (uint32_t)enabled_extensions.size(),
            .ppEnabledExtensionNames = enabled_extensions.data(),
        };

        vk::Device device = m_physical_device.createDevice(device_createinfo);

#if (VULKAN_HPP_DISPATCH_LOADER_DYNAMIC == 1)
        // initialize function pointers for instance
        VULKAN_HPP_DEFAULT_DISPATCHER.init(device);
#endif

        m_device = device;
    }

    void Device::create_vma_allocator()
    {
        vma::AllocatorCreateInfo createinfo;
        // set allocator properties
        createinfo.physicalDevice   = m_physical_device;
        createinfo.device           = m_device;
        createinfo.instance         = *m_instance;
        createinfo.vulkanApiVersion = m_req_api_version;

        m_vma_allocator = vma::createAllocator(createinfo);
    }

    void Device::create_command_buffer()
    {
        // create a CommandPool to allocate a CommandBuffer from
        vk::CommandPoolCreateInfo command_pool_info{
            .flags = vk::CommandPoolCreateFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer), .queueFamilyIndex = m_graphics_queue_family_index};
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

    void Device::create_swapchain()
    {
        // create Surface from Win32;
        vk::Win32SurfaceCreateInfoKHR win32_surface_createinfo{
            .flags     = vk::Win32SurfaceCreateFlagsKHR(),
            .hinstance = m_hinstance,
            .hwnd      = m_hwnd,
        };

        m_surface   = m_instance->createWin32SurfaceKHR(win32_surface_createinfo);
        auto&& rect = get_window_rect();

        // get the supported VkFormats
        std::vector<vk::SurfaceFormatKHR> formats = m_physical_device.getSurfaceFormatsKHR(m_surface);
        assert(!formats.empty());
        vk::Format format = (formats[0].format == vk::Format::eUndefined) ? vk::Format::eB8G8R8A8Unorm : formats[0].format;

        vk::SurfaceCapabilitiesKHR surface_capabilities = m_physical_device.getSurfaceCapabilitiesKHR(m_surface);
        vk::Extent2D               swapchain_extent;
        if (surface_capabilities.currentExtent.width == std::numeric_limits<uint32_t>::max()) {
            // If the surface size is undefined, the size is set to the size of the images requested.
            swapchain_extent.width = std::clamp((uint32_t)rect.Width(), surface_capabilities.minImageExtent.width, surface_capabilities.maxImageExtent.width);
            swapchain_extent.height =
                std::clamp((uint32_t)rect.Height(), surface_capabilities.minImageExtent.height, surface_capabilities.maxImageExtent.height);
        }
        else {
            // If the surface size is defined, the swap chain size must match
            swapchain_extent = surface_capabilities.currentExtent;
        }

        m_swapchain_image_size = swapchain_extent;

        // The FIFO present mode is guaranteed by the spec to be supported
        vk::PresentModeKHR swapchain_present_mode = vk::PresentModeKHR::eFifo;

        // Determine the number of VkImage's to use in the swapchain.
        // Ideally, we desire to own 1 image at a time, the rest of the images can
        // either be rendered to and/or being queued up for display.
        uint32_t desired_swapchain_images = surface_capabilities.minImageCount + 1;
        if ((surface_capabilities.maxImageCount > 0) && (desired_swapchain_images > surface_capabilities.maxImageCount)) {
            // Application must settle for fewer images than desired.
            desired_swapchain_images = surface_capabilities.maxImageCount;
        }

        vk::SurfaceTransformFlagBitsKHR pre_transform = (surface_capabilities.supportedTransforms & vk::SurfaceTransformFlagBitsKHR::eIdentity)
                                                            ? vk::SurfaceTransformFlagBitsKHR::eIdentity
                                                            : surface_capabilities.currentTransform;

        vk::CompositeAlphaFlagBitsKHR composite_alpha =
            (surface_capabilities.supportedCompositeAlpha & vk::CompositeAlphaFlagBitsKHR::ePreMultiplied)    ? vk::CompositeAlphaFlagBitsKHR::ePreMultiplied
            : (surface_capabilities.supportedCompositeAlpha & vk::CompositeAlphaFlagBitsKHR::ePostMultiplied) ? vk::CompositeAlphaFlagBitsKHR::ePostMultiplied
            : (surface_capabilities.supportedCompositeAlpha & vk::CompositeAlphaFlagBitsKHR::eInherit)        ? vk::CompositeAlphaFlagBitsKHR::eInherit
                                                                                                              : vk::CompositeAlphaFlagBitsKHR::eOpaque;

        auto&& prev_swapchain = m_swapchain;

        vk::SwapchainCreateInfoKHR swapchain_create_info{
            .flags            = vk::SwapchainCreateFlagsKHR(),
            .surface          = m_surface,
            .minImageCount    = desired_swapchain_images,
            .imageFormat      = format,
            .imageColorSpace  = vk::ColorSpaceKHR::eSrgbNonlinear, // todo: should get from surface caps
            .imageExtent      = swapchain_extent,
            .imageArrayLayers = 1, // Number of layers in each image (usually 1 unless stereoscopic)
            .imageUsage       = vk::ImageUsageFlagBits::eColorAttachment,
            .imageSharingMode = vk::SharingMode::eExclusive, // Access mode of the images (exclusive to one queue family)
            .preTransform     = pre_transform,
            .compositeAlpha   = composite_alpha,
            .presentMode      = swapchain_present_mode,
            .clipped          = true, // Whether to clip obscured pixels (improves performance)
            .oldSwapchain     = prev_swapchain,
        };

        // find Present queue family index
        m_present_queue_family_index = find_present_queue_family_index();

        uint32_t queue_family_indices[2] = {m_graphics_queue_family_index, m_present_queue_family_index};
        if (m_graphics_queue_family_index != m_present_queue_family_index) {
            // If the graphics and present queues are from different queue families, we either have to explicitly transfer
            // ownership of images between the queues, or we have to create the swapchain with imageSharingMode as
            // VK_SHARING_MODE_CONCURRENT
            swapchain_create_info.imageSharingMode      = vk::SharingMode::eConcurrent;
            swapchain_create_info.queueFamilyIndexCount = 2;
            swapchain_create_info.pQueueFamilyIndices   = queue_family_indices;
        }

        m_swapchain = m_device.createSwapchainKHR(swapchain_create_info);

        std::vector<vk::Image> swapchain_images = m_device.getSwapchainImagesKHR(m_swapchain);

        auto&& image_views = m_swapchain_image_views;
        image_views.reserve(swapchain_images.size());

        vk::ImageViewCreateInfo image_view_create_info{
            .flags    = {},
            .viewType = vk::ImageViewType::e2D,
            .format   = format,
            .subresourceRange =
                vk::ImageSubresourceRange{
                    .aspectMask     = vk::ImageAspectFlagBits::eColor,
                    .baseMipLevel   = 0,
                    .levelCount     = 1,
                    .baseArrayLayer = 0,
                    .layerCount     = 1,
                },
        };

        for (auto&& image : swapchain_images) {
            image_view_create_info.image = image;
            image_views.push_back(m_device.createImageView(image_view_create_info));
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
            .flags       = vk::ImageCreateFlags(),
            .imageType   = vk::ImageType::e2D,
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

        std::tie(m_depth_buffer.m_image, m_depth_buffer.m_alloc) = m_vma_allocator.createImage(image_createinfo, alloc_createinfo);

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
            vk::Format color_format = pick_surface_format(m_physical_device.getSurfaceFormatsKHR(m_surface)).format;
            auto&&	   depth_format = m_depth_buffer.m_format;

            // Render pass ----------
            std::array<vk::AttachmentDescription, 2> attachment_desc;
            attachment_desc[0] = vk::AttachmentDescription(vk::AttachmentDescriptionFlags(), color_format, vk::SampleCountFlagBits::e1,
                    vk::AttachmentLoadOp::eClear, vk::AttachmentStoreOp::eStore, vk::AttachmentLoadOp::eDontCare, vk::AttachmentStoreOp::eDontCare,
                    vk::ImageLayout::eUndefined, vk::ImageLayout::ePresentSrcKHR);
            attachment_desc[1] = vk::AttachmentDescription(vk::AttachmentDescriptionFlags(), depth_format, vk::SampleCountFlagBits::e1,
                    vk::AttachmentLoadOp::eClear, vk::AttachmentStoreOp::eDontCare, vk::AttachmentLoadOp::eDontCare, vk::AttachmentStoreOp::eDontCare,
                    vk::ImageLayout::eUndefined, vk::ImageLayout::eDepthStencilAttachmentOptimal);

            vk::AttachmentReference color_reference(0, vk::ImageLayout::eColorAttachmentOptimal);
            vk::AttachmentReference depth_reference(1, vk::ImageLayout::eDepthStencilAttachmentOptimal);
            vk::SubpassDescription	subpass(vk::SubpassDescriptionFlags(), vk::PipelineBindPoint::eGraphics, {}, color_reference, {}, &depth_reference);

            m_render_pass = m_device.createRenderPass(vk::RenderPassCreateInfo(vk::RenderPassCreateFlags(), attachment_desc, subpass));
            // ----------

            // Note: if multiple RT is used, attachments should equal numRT+Depth
            // Frame buffer for Back buffers
            vk::ImageView attachments[2];
            attachments[1] = m_depth_buffer.m_view;

            auto&& r = get_window_rect();

            vk::FramebufferCreateInfo framebuffer_createinfo(
                    vk::FramebufferCreateFlags(), m_render_pass, m_depth_buffer.m_view ? 2 : 1, attachments, r.Width(), r.Height(), 1);

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
    //     vk::ResultValue<uint32_t> current_buffer = m_device.acquireNextImageKHR(m_swapchain, m_fence_timeout, image_available_semaphore, nullptr);
    //     assert(current_buffer.result == vk::Result::eSuccess);
    //     assert(current_buffer.value < m_frame_buffers.size());
    //
    //     command_buffer.begin(vk::CommandBufferBeginInfo(vk::CommandBufferUsageFlags()));
    //
    //     std::array<vk::ClearValue, 2> clear_values;
    //     clear_values[0].color        = vk::ClearColorValue(std::array<float, 4>({{0.2f, 0.2f, 0.2f, 0.2f}}));
    //     clear_values[1].depthStencil = vk::ClearDepthStencilValue(1.0f, 0);
    //
    //     auto&& r        = get_window_rect();
    //     auto&& extent2d = vk::Extent2D(r.Width(), r.Height());
    //
    //     vk::RenderPassBeginInfo renderPassBeginInfo(
    //         m_render_pass, m_frame_buffers[current_buffer.value], vk::Rect2D(vk::Offset2D(0, 0), extent2d), clear_values);
    //     command_buffer.beginRenderPass(renderPassBeginInfo, vk::SubpassContents::eInline);
    //     command_buffer.bindPipeline(vk::PipelineBindPoint::eGraphics, m_pipeline);
    //     // m_command_buffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, pipelineLayout, 0, descriptorSet, nullptr);
    //
    //     // m_command_buffer.bindVertexBuffers(0, vertexBufferData.buffer, {0});
    //     command_buffer.setViewport(0, vk::Viewport(0.0f, 0.0f, static_cast<float>(extent2d.width), static_cast<float>(extent2d.height), 0.0f, 1.0f));
    //     command_buffer.setScissor(0, vk::Rect2D(vk::Offset2D(0, 0), extent2d));
    //
    //     // m_command_buffer.draw(12 * 3, 1, 0, 0);
    //     command_buffer.draw(3, 1, 0, 0);
    //     command_buffer.endRenderPass();
    //     command_buffer.end();
    //
    //     vk::PipelineStageFlags wait_destination_stage_mask(vk::PipelineStageFlagBits::eColorAttachmentOutput);
    //     vk::SubmitInfo         submit_info(image_available_semaphore, wait_destination_stage_mask, command_buffer);
    //
    //     vk::Queue graphics_queue = m_device.getQueue(m_graphics_queue_family_index, 0);
    //     graphics_queue.submit(submit_info, inflight_fence);
    //
    //     while (vk::Result::eTimeout == m_device.waitForFences(inflight_fence, VK_TRUE, m_fence_timeout)) {
    //     };
    //
    //     vk::Queue  present_queue = m_device.getQueue(m_present_queue_family_index, 0);
    //     vk::Result result        = present_queue.presentKHR(vk::PresentInfoKHR({}, m_swapchain, current_buffer.value));
    //     switch (result) {
    //     case vk::Result::eSuccess:
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
        // clear_values[0].color		 = vk::ClearColorValue(std::array<float, 4>({{0.2f, 0.2f, 0.2f, 0.2f}}));
        // clear_values[1].depthStencil = vk::ClearDepthStencilValue(1.0f, 0);
        // vk::RenderPassBeginInfo renderPassBeginInfo(
        //	m_render_pass, m_frame_buffers[m_swapchain_buffer_idx], vk::Rect2D(vk::Offset2D(0, 0), m_swapchain_image_size), clear_values);
        // command_buffer->beginRenderPass(renderPassBeginInfo, vk::SubpassContents::eInline);

        vk::ClearColorValue        clear_colour{{{0.2f, 0.2f, 0.2f, 0.2f}}};
        vk::ClearDepthStencilValue clear_depth = {.depth = 1.0f, .stencil = 0u};

        command_buffer->setViewport(
            0, vk::Viewport(0.0f, 0.0f, static_cast<float>(m_swapchain_image_size.width), static_cast<float>(m_swapchain_image_size.height), 0.0f, 1.0f));
        command_buffer->setScissor(0, vk::Rect2D(vk::Offset2D(0, 0), m_swapchain_image_size));

        auto&& t0 = m_shader_manager->get_technique("t0").lock();
        command_buffer->bindPipeline(vk::PipelineBindPoint::eGraphics, t0->m_pipeline);
        // m_command_buffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, pipelineLayout, 0, descriptorSet, nullptr);

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

        command_buffer->endRenderPass();

        end_frame();
    }

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
        result = m_device.acquireNextImageKHR(m_swapchain, m_fence_timeout, image_available_semaphore, nullptr, &m_swapchain_buffer_idx);
        if (result == vk::Result::eErrorOutOfDateKHR) {
            auto&& command_buffer = m_frame_resource[frame_resource_idx]->m_command_buffer;
            command_buffer.reset();
            // recreate swapchain
            return;
        }
        else if (result != vk::Result::eSuccess) {
            throw std::runtime_error("failed to acquire swap chain image!");
        }

        assert(m_swapchain_buffer_idx < m_frame_buffers.size());

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

    vk::Format Device::get_backbuffer_colour_format() const { return pick_surface_format(m_physical_device.getSurfaceFormatsKHR(m_surface)).format; }

    vk::Format Device::get_backbuffer_depth_format() const { return m_depth_buffer.m_format; }

    void Device::transition_image_layout(vk::Image image, vk::ImageLayout dst_layout, vk::ImageLayout src_layout, vk::AccessFlags2 dst_access_flags,
        vk::AccessFlags2 src_access_flags, vk::PipelineStageFlags2 dst_stage_flags, vk::PipelineStageFlags2 src_stage_flags)
    {
        vk::ImageMemoryBarrier2 image_barrier{ // Specify the pipeline stages and access masks for the barrier
            .srcStageMask  = src_stage_flags,  // Source pipeline stage mask
            .srcAccessMask = src_access_flags, // Source access mask
            .dstStageMask  = dst_stage_flags,  // Destination pipeline stage mask
            .dstAccessMask = dst_access_flags, // Destination access mask

            // Specify the old and new layouts of the image
            .oldLayout = src_layout, // Current layout of the image
            .newLayout = dst_layout, // Target layout of the image

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

    std::vector<const char*> Device::gather_layers(const std::vector<std::string>& layers, const std::vector<vk::LayerProperties>& layer_properties)
    {
        std::vector<char const*> enabled_layers;
        enabled_layers.reserve(layers.size());

        for (auto&& layer : layers) {
            assert(std::find_if(layer_properties.begin(), layer_properties.end(), [layer](const vk::LayerProperties& lp) { return layer == lp.layerName; }) !=
                   layer_properties.end());
            enabled_layers.push_back(layer.data());
        }

        // Enable standard validation layer to find as much errors as possible!
        if (std::find(layers.begin(), layers.end(), "VK_LAYER_KHRONOS_validation") == layers.end() &&
            std::find_if(layer_properties.begin(), layer_properties.end(),
                [](const vk::LayerProperties& lp) { return (strcmp("VK_LAYER_KHRONOS_validation", lp.layerName) == 0); }) != layer_properties.end()) {
            enabled_layers.push_back("VK_LAYER_KHRONOS_validation");
        }

        return enabled_layers;
    }

    std::vector<const char*> Device::gather_extensions(
        const std::vector<std::string>& extensions, const std::vector<vk::ExtensionProperties>& extension_properties)
    {
        std::vector<char const*> enabled_extensions;
        enabled_extensions.reserve(extensions.size());
        for (const auto& ext : extensions) {
            assert(std::find_if(extension_properties.begin(), extension_properties.end(),
                       [ext](const vk::ExtensionProperties& ep) { return ext == ep.extensionName; }) != extension_properties.end());
            enabled_extensions.push_back(ext.data());
        }
#if !defined(NDEBUG)
        if (std::find(extensions.begin(), extensions.end(), VK_EXT_DEBUG_UTILS_EXTENSION_NAME) == extensions.end() &&
            std::find_if(extension_properties.begin(), extension_properties.end(), [](const vk::ExtensionProperties& ep) {
                return (strcmp(VK_EXT_DEBUG_UTILS_EXTENSION_NAME, ep.extensionName) == 0);
            }) != extension_properties.end()) {
            enabled_extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        }
#endif
        return enabled_extensions;
    }

    vk::StructureChain<vk::InstanceCreateInfo, vk::DebugUtilsMessengerCreateInfoEXT> Device::make_instance_create_info_chain(
        const vk::ApplicationInfo& application_info, const std::vector<const char*>& layers, const std::vector<const char*>& extensions)
    {
        // in non-debug mode just use the InstanceCreateInfo for instance creation
        // vk::StructureChain<vk::InstanceCreateInfo> instance_create_info({{}, &application_info, layers, extensions});

        // in debug mode, addionally use the debugUtilsMessengerCallback in instance creation!
        vk::DebugUtilsMessageSeverityFlagsEXT severityFlags(
            vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning | vk::DebugUtilsMessageSeverityFlagBitsEXT::eError);
        vk::DebugUtilsMessageTypeFlagsEXT messageTypeFlags(vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral | vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance |
                                                           vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation);

        vk::InstanceCreateInfo instance_createinfo{
            .pApplicationInfo        = &application_info,
            .enabledLayerCount       = (uint32_t)layers.size(),
            .ppEnabledLayerNames     = layers.data(),
            .enabledExtensionCount   = (uint32_t)extensions.size(),
            .ppEnabledExtensionNames = extensions.data(),
        };

        vk::DebugUtilsMessengerCreateInfoEXT debug_util_createinfo{
            .messageSeverity = severityFlags,
            .messageType     = messageTypeFlags,
            .pfnUserCallback = debug_utils_messenger_callback,
        };

        vk::StructureChain<vk::InstanceCreateInfo, vk::DebugUtilsMessengerCreateInfoEXT> complete_instance_create_info{
            instance_createinfo,
            debug_util_createinfo,
        };

        return complete_instance_create_info;
    }

    vk::DebugUtilsMessengerEXT Device::create_debug_utils_messenger_EXT(const vk::Instance& instance)
    {
        return instance.createDebugUtilsMessengerEXT(make_debug_utils_messenger_create_info_EXT());
    }

    vk::DebugUtilsMessengerCreateInfoEXT Device::make_debug_utils_messenger_create_info_EXT()
    {
        vk::DebugUtilsMessageSeverityFlagsEXT severityFlags(
            vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning | vk::DebugUtilsMessageSeverityFlagBitsEXT::eError);
        vk::DebugUtilsMessageTypeFlagsEXT messageTypeFlags(vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral | vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance |
                                                           vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation);

        return vk::DebugUtilsMessengerCreateInfoEXT{
            .flags = {}, .messageSeverity = severityFlags, .messageType = messageTypeFlags, .pfnUserCallback = debug_utils_messenger_callback};
    }

    VKAPI_ATTR VkBool32 VKAPI_CALL Device::debug_utils_messenger_callback(vk::DebugUtilsMessageSeverityFlagBitsEXT message_severity,
        vk::DebugUtilsMessageTypeFlagsEXT message_types, const vk::DebugUtilsMessengerCallbackDataEXT* p_callback_data, void*)
    {
#if !defined(NDEBUG)
        if (p_callback_data->messageIdNumber == 648835635) {
            // UNASSIGNED-khronos-Validation-debug-build-warning-message
            return VK_FALSE;
        }
        if (p_callback_data->messageIdNumber == 767975156) {
            // UNASSIGNED-BestPractices-vkCreateInstance-specialuse-extension
            return VK_FALSE;
        }
#endif

        std::cerr << vk::to_string(static_cast<vk::DebugUtilsMessageSeverityFlagBitsEXT>(message_severity)) << ": "
                  << vk::to_string(static_cast<vk::DebugUtilsMessageTypeFlagsEXT>(message_types)) << ":\n";
        std::cerr << "\t"
                  << "messageIDName   = <" << p_callback_data->pMessageIdName << ">\n";
        std::cerr << "\t"
                  << "messageIdNumber = " << p_callback_data->messageIdNumber << "\n";
        std::cerr << "\t"
                  << "message         = <" << p_callback_data->pMessage << ">\n";
        if (0 < p_callback_data->queueLabelCount) {
            std::cerr << "\t"
                      << "Queue Labels:\n";
            for (uint32_t i = 0; i < p_callback_data->queueLabelCount; i++) {
                std::cerr << "\t\t"
                          << "labelName = <" << p_callback_data->pQueueLabels[i].pLabelName << ">\n";
            }
        }
        if (0 < p_callback_data->cmdBufLabelCount) {
            std::cerr << "\t"
                      << "CommandBuffer Labels:\n";
            for (uint32_t i = 0; i < p_callback_data->cmdBufLabelCount; i++) {
                std::cerr << "\t\t"
                          << "labelName = <" << p_callback_data->pCmdBufLabels[i].pLabelName << ">\n";
            }
        }
        if (0 < p_callback_data->objectCount) {
            std::cerr << "\t"
                      << "Objects:\n";
            for (uint32_t i = 0; i < p_callback_data->objectCount; i++) {
                std::cerr << "\t\t"
                          << "Object " << i << "\n";
                std::cerr << "\t\t\t"
                          << "objectType   = " << vk::to_string(static_cast<vk::ObjectType>(p_callback_data->pObjects[i].objectType)) << "\n";
                std::cerr << "\t\t\t"
                          << "objectHandle = " << p_callback_data->pObjects[i].objectHandle << "\n";
                if (p_callback_data->pObjects[i].pObjectName) {
                    std::cerr << "\t\t\t"
                              << "objectName   = <" << p_callback_data->pObjects[i].pObjectName << ">\n";
                }
            }
        }
        return VK_TRUE;
    }

    uint32_t Device::find_graphics_queue_family_index(const std::vector<vk::QueueFamilyProperties>& queue_family_properties)
    {
        // get the first index into queueFamiliyProperties which supports graphics
        std::vector<vk::QueueFamilyProperties>::const_iterator graphics_queue_family_property = std::find_if(queue_family_properties.begin(),
            queue_family_properties.end(), [](vk::QueueFamilyProperties const& qfp) { return qfp.queueFlags & vk::QueueFlagBits::eGraphics; });
        assert(graphics_queue_family_property != queue_family_properties.end());
        return static_cast<uint32_t>(std::distance(queue_family_properties.begin(), graphics_queue_family_property));
    }

    uint32_t Device::find_present_queue_family_index()
    {
        // get the QueueFamilyProperties of the first PhysicalDevice
        std::vector<vk::QueueFamilyProperties> queue_family_properties = m_physical_device.getQueueFamilyProperties();

        // determine a queueFamilyIndex that suports present
        // first check if the graphicsQueueFamiliyIndex is good enough
        uint32_t present_queue_family_index = m_physical_device.getSurfaceSupportKHR(m_graphics_queue_family_index, m_surface)
                                                  ? m_graphics_queue_family_index
                                                  : (uint32_t)queue_family_properties.size();
        if (present_queue_family_index == queue_family_properties.size()) {
            // the graphicsQueueFamilyIndex doesn't support present -> look for an other family index that supports both
            // graphics and present
            for (size_t i = 0; i < queue_family_properties.size(); i++) {
                if ((queue_family_properties[i].queueFlags & vk::QueueFlagBits::eGraphics) &&
                    m_physical_device.getSurfaceSupportKHR(static_cast<uint32_t>(i), m_surface)) {
                    m_graphics_queue_family_index = static_cast<uint32_t>(i);
                    present_queue_family_index    = static_cast<uint32_t>(i);
                    break;
                }
            }
            if (present_queue_family_index == queue_family_properties.size()) {
                // there's nothing like a single family index that supports both graphics and present -> look for an other
                // family index that supports present
                for (size_t i = 0; i < queue_family_properties.size(); i++) {
                    if (m_physical_device.getSurfaceSupportKHR(static_cast<uint32_t>(i), m_surface)) {
                        present_queue_family_index = static_cast<uint32_t>(i);

                        break;
                    }
                }
            }
        }
        if ((m_graphics_queue_family_index == queue_family_properties.size()) || (present_queue_family_index == queue_family_properties.size())) {
            throw std::runtime_error("Could not find a queue for graphics or present -> terminating");
        }

        return present_queue_family_index;
    }

    std::vector<std::string> Device::get_instance_extensions()
    {
        std::vector<std::string> extensions = {VK_KHR_SURFACE_EXTENSION_NAME
#if defined(VK_USE_PLATFORM_WIN32_KHR)
            ,
            VK_KHR_WIN32_SURFACE_EXTENSION_NAME
#endif
            ,
            VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME};
        return extensions;
    }

    bool Device::is_instance_extension_enabled(const std::string& name)
    {
        auto&& instance_extensions = get_instance_extensions();
        return std::find_if(instance_extensions.begin(), instance_extensions.end(),
                   [name](const std::string enabled_extension) { return strcmp(name.c_str(), enabled_extension.c_str()) == 0; }) != instance_extensions.end();
    }

    std::vector<std::string> Device::get_device_extensions() { return {VK_KHR_SWAPCHAIN_EXTENSION_NAME, VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME}; }

    uint32_t Device::curr_frame_resource_idx() { return m_frame_count % MAX_FRAMES_IN_FLIGHT; }

    Frame_resource& Device::curr_frame_resource()
    {
        auto&& frame_resource_idx = curr_frame_resource_idx();
        return *m_frame_resource[frame_resource_idx];
    }

    vk::CommandBuffer* Device::curr_command_buffer()
    {
        // note: need to address when we are recording using m_single_command as well
        auto&& frame_resource_idx = curr_frame_resource_idx();

        return m_frame_resource[frame_resource_idx]->m_command_buffer_opened ? &m_frame_resource[frame_resource_idx]->m_command_buffer : nullptr;
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

                auto it = std::find_if(formats.begin(), formats.end(), [requested_format, requested_color_space](vk::SurfaceFormatKHR const& f) {
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
