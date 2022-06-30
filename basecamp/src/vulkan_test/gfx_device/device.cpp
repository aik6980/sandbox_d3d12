#include "device.h"

#include <thread>

VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE

namespace VKN {

Device::Device() : m_vertex_shader(*this), m_pixel_shader(*this), m_vertex_shader_2(*this) {}

void Device::create(HINSTANCE hinstance, HWND hwnd)
{
    m_hinstance = hinstance;
    m_hwnd      = hwnd;

    try {
        create_instance();
        create_device();
        create_vma_allocator();
        create_command_buffer();
        create_swapchain();

        create_depth_buffer();

        create_sync_object();

        create_render_pass();

        m_vertex_shader.create_shader("hello_triangle.vs");
        m_vertex_shader_2.create_shader("hello_triangle_mesh.vs");
        m_pixel_shader.create_shader("hello_triangle.ps");

        create_pipeline_state_object();
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

    // destroy pipeline
    m_device.destroyPipeline(m_pipeline);

    // destroy render pass
    for (auto framebuffer : m_frame_buffers) {
        m_device.destroyFramebuffer(framebuffer);
    }
    m_device.destroyRenderPass(m_render_pass);

    // destroy shader
    m_pixel_shader.destroy();
    m_vertex_shader_2.destroy();
    m_vertex_shader.destroy();

    // destroy resources
    destroy_resource(m_depth_buffer);

    // destroy the imageViews, the swapChain,and the surface
    for (auto& view : m_swapchain_image_views) {
        m_device.destroyImageView(view);
    }
    m_device.destroySwapchainKHR(m_swapchain);
    m_instance.destroySurfaceKHR(m_surface);

    // destroy command buffer
    // freeing the commandBuffer is optional, as it will automatically freed when the corresponding CommandPool is destroyed.
    for (uint32_t i = 0; i < m_frame_resource.size(); ++i) {
        m_device.freeCommandBuffers(m_command_pool, m_frame_resource[i].m_command_buffer);
    }
    // destroy the commandPool
    m_device.destroyCommandPool(m_command_pool);

    // vma allocator
    m_vma_allocator.destroy();

    // destroy the device
    m_device.destroy();

    // destroy
    m_instance.destroyDebugUtilsMessengerEXT(m_debug_utils_messenger);
    m_instance.destroy();
}

void Device::create_instance()
{
    // init paramaters
    const static auto                     app_name    = std::string("vulkan_gfx");
    const static auto                     api_version = m_req_api_version;
    const static std::vector<std::string> layers      = {};
    const static std::vector<std::string> extensions  = get_instance_extensions();

#if (VULKAN_HPP_DISPATCH_LOADER_DYNAMIC == 1)
    static vk::DynamicLoader  dl;
    PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr = dl.getProcAddress<PFN_vkGetInstanceProcAddr>("vkGetInstanceProcAddr");
    VULKAN_HPP_DEFAULT_DISPATCHER.init(vkGetInstanceProcAddr);
#endif

    vk::ApplicationInfo applicationInfo(app_name.c_str(), 1, app_name.c_str(), 1, api_version);

    std::vector<const char*> enabled_layers     = gather_layers(layers, vk::enumerateInstanceLayerProperties());
    std::vector<const char*> enabled_extensions = gather_extensions(extensions, vk::enumerateInstanceExtensionProperties());

    // create instance
    m_instance = vk::createInstance(make_instance_create_info_chain(applicationInfo, enabled_layers, enabled_extensions).get<vk::InstanceCreateInfo>());
#if (VULKAN_HPP_DISPATCH_LOADER_DYNAMIC == 1)
    // initialize function pointers for instance
    VULKAN_HPP_DEFAULT_DISPATCHER.init(m_instance);
#endif

    // create debug layer
    m_debug_utils_messenger = create_debug_utils_messenger_EXT(m_instance);
}

void Device::create_device()
{
    // enumerate the physical devices
    m_physical_device = m_instance.enumeratePhysicalDevices().front();

    // get the QueueFamilyProperties of the first PhysicalDevice
    std::vector<vk::QueueFamilyProperties> queue_family_properties = m_physical_device.getQueueFamilyProperties();

    size_t graphics_queue_family_index = find_graphics_queue_family_index(queue_family_properties);
    assert(graphics_queue_family_index < queue_family_properties.size());
    m_graphics_queue_family_index = static_cast<uint32_t>(graphics_queue_family_index);

    // find device extension
    auto&& device_extensions = get_device_extensions();

    std::vector<const char*> enabled_extensions;
    enabled_extensions.reserve(device_extensions.size());
    for (auto&& ext : device_extensions) {
        enabled_extensions.push_back(ext.data());
    }

    // create a Device
    float                     queue_priority = 0.0f;
    vk::DeviceQueueCreateInfo device_queue_createinfo(vk::DeviceQueueCreateFlags(), m_graphics_queue_family_index, 1, &queue_priority);
    vk::DeviceCreateInfo      device_createinfo({}, device_queue_createinfo, {}, enabled_extensions, nullptr);

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
    createinfo.instance         = m_instance;
    createinfo.vulkanApiVersion = m_req_api_version;

    m_vma_allocator = vma::createAllocator(createinfo);
}

void Device::create_command_buffer()
{
    // create a CommandPool to allocate a CommandBuffer from
    m_command_pool = m_device.createCommandPool(
        vk::CommandPoolCreateInfo(vk::CommandPoolCreateFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer), m_graphics_queue_family_index));

    // allocate a CommandBuffer from the CommandPool
    auto&& command_buffers =
        m_device.allocateCommandBuffers(vk::CommandBufferAllocateInfo(m_command_pool, vk::CommandBufferLevel::ePrimary, MAX_FRAMES_IN_FLIGHT));
    for (uint32_t i = 0; i < m_frame_resource.size(); ++i) {
        m_frame_resource[i].m_command_buffer = command_buffers[i];
    }
}

void Device::create_swapchain()
{
    // create Surface from Win32;
    m_surface   = m_instance.createWin32SurfaceKHR(vk::Win32SurfaceCreateInfoKHR(vk::Win32SurfaceCreateFlagsKHR(), m_hinstance, m_hwnd));
    auto&& rect = get_window_rect();

    // get the supported VkFormats
    std::vector<vk::SurfaceFormatKHR> formats = m_physical_device.getSurfaceFormatsKHR(m_surface);
    assert(!formats.empty());
    vk::Format format = (formats[0].format == vk::Format::eUndefined) ? vk::Format::eB8G8R8A8Unorm : formats[0].format;

    vk::SurfaceCapabilitiesKHR surface_capabilities = m_physical_device.getSurfaceCapabilitiesKHR(m_surface);
    vk::Extent2D               swapchain_extent;
    if (surface_capabilities.currentExtent.width == std::numeric_limits<uint32_t>::max()) {
        // If the surface size is undefined, the size is set to the size of the images requested.
        swapchain_extent.width  = std::clamp((uint32_t)rect.Width(), surface_capabilities.minImageExtent.width, surface_capabilities.maxImageExtent.width);
        swapchain_extent.height = std::clamp((uint32_t)rect.Height(), surface_capabilities.minImageExtent.height, surface_capabilities.maxImageExtent.height);
    }
    else {
        // If the surface size is defined, the swap chain size must match
        swapchain_extent = surface_capabilities.currentExtent;
    }

    m_swapchain_image_size = swapchain_extent;

    // The FIFO present mode is guaranteed by the spec to be supported
    vk::PresentModeKHR swapchain_present_mode = vk::PresentModeKHR::eFifo;

    vk::SurfaceTransformFlagBitsKHR pre_transform = (surface_capabilities.supportedTransforms & vk::SurfaceTransformFlagBitsKHR::eIdentity)
                                                        ? vk::SurfaceTransformFlagBitsKHR::eIdentity
                                                        : surface_capabilities.currentTransform;

    vk::CompositeAlphaFlagBitsKHR composite_alpha =
        (surface_capabilities.supportedCompositeAlpha & vk::CompositeAlphaFlagBitsKHR::ePreMultiplied)    ? vk::CompositeAlphaFlagBitsKHR::ePreMultiplied
        : (surface_capabilities.supportedCompositeAlpha & vk::CompositeAlphaFlagBitsKHR::ePostMultiplied) ? vk::CompositeAlphaFlagBitsKHR::ePostMultiplied
        : (surface_capabilities.supportedCompositeAlpha & vk::CompositeAlphaFlagBitsKHR::eInherit)        ? vk::CompositeAlphaFlagBitsKHR::eInherit
                                                                                                          : vk::CompositeAlphaFlagBitsKHR::eOpaque;

    vk::SwapchainCreateInfoKHR swapchain_create_info(vk::SwapchainCreateFlagsKHR(), m_surface, surface_capabilities.minImageCount, format,
        vk::ColorSpaceKHR::eSrgbNonlinear, swapchain_extent, 1, vk::ImageUsageFlagBits::eColorAttachment, vk::SharingMode::eExclusive, {}, pre_transform,
        composite_alpha, swapchain_present_mode, true, nullptr);

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
    vk::ImageViewCreateInfo image_view_create_info({}, {}, vk::ImageViewType::e2D, format, {}, {vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1});
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

    vk::ImageCreateInfo image_createinfo(vk::ImageCreateFlags(), vk::ImageType::e2D, depth_format, vk::Extent3D(r.Width(), r.Height(), 1), 1, 1,
        vk::SampleCountFlagBits::e1, tiling, vk::ImageUsageFlagBits::eDepthStencilAttachment);

    vma::AllocationCreateInfo alloc_createinfo;
    alloc_createinfo.usage = vma::MemoryUsage::eAuto;

    m_depth_buffer.m_format = depth_format;

    std::tie(m_depth_buffer.m_image, m_depth_buffer.m_alloc) = m_vma_allocator.createImage(image_createinfo, alloc_createinfo);

    m_depth_buffer.m_view = m_device.createImageView(vk::ImageViewCreateInfo(
        vk::ImageViewCreateFlags(), m_depth_buffer.m_image, vk::ImageViewType::e2D, depth_format, {}, {vk::ImageAspectFlagBits::eDepth, 0, 1, 0, 1}));
}

void Device::create_render_pass()
{
    vk::Format color_format = pick_surface_format(m_physical_device.getSurfaceFormatsKHR(m_surface)).format;
    auto&&     depth_format = m_depth_buffer.m_format;

    // Render pass ----------
    std::array<vk::AttachmentDescription, 2> attachment_desc;
    attachment_desc[0] = vk::AttachmentDescription(vk::AttachmentDescriptionFlags(), color_format, vk::SampleCountFlagBits::e1, vk::AttachmentLoadOp::eClear,
        vk::AttachmentStoreOp::eStore, vk::AttachmentLoadOp::eDontCare, vk::AttachmentStoreOp::eDontCare, vk::ImageLayout::eUndefined,
        vk::ImageLayout::ePresentSrcKHR);
    attachment_desc[1] = vk::AttachmentDescription(vk::AttachmentDescriptionFlags(), depth_format, vk::SampleCountFlagBits::e1, vk::AttachmentLoadOp::eClear,
        vk::AttachmentStoreOp::eDontCare, vk::AttachmentLoadOp::eDontCare, vk::AttachmentStoreOp::eDontCare, vk::ImageLayout::eUndefined,
        vk::ImageLayout::eDepthStencilAttachmentOptimal);

    vk::AttachmentReference color_reference(0, vk::ImageLayout::eColorAttachmentOptimal);
    vk::AttachmentReference depth_reference(1, vk::ImageLayout::eDepthStencilAttachmentOptimal);
    vk::SubpassDescription  subpass(vk::SubpassDescriptionFlags(), vk::PipelineBindPoint::eGraphics, {}, color_reference, {}, &depth_reference);

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

void Device::create_sync_object()
{
    for (uint32_t i = 0; i < m_frame_resource.size(); ++i) {
        m_frame_resource[i].m_image_available_semaphore = m_device.createSemaphore(vk::SemaphoreCreateInfo());
        m_frame_resource[i].m_render_finished_semaphore = m_device.createSemaphore(vk::SemaphoreCreateInfo());
        // initialize with the Signaled state
        m_frame_resource[i].m_inflight_fence = m_device.createFence(vk::FenceCreateInfo(vk::FenceCreateFlagBits::eSignaled));
    }
}

void Device::destroy_sync_object()
{
    for (uint32_t i = 0; i < m_frame_resource.size(); ++i) {
        m_device.destroyFence(m_frame_resource[i].m_inflight_fence);
        m_device.destroySemaphore(m_frame_resource[i].m_render_finished_semaphore);
        m_device.destroySemaphore(m_frame_resource[i].m_image_available_semaphore);
    }
}

void Device::draw_once()
{
    auto&& image_available_semaphore = m_frame_resource[0].m_image_available_semaphore;
    auto&& inflight_fence            = m_frame_resource[0].m_inflight_fence;

    auto&& command_buffer = m_frame_resource[0].m_command_buffer;

    // Get the index of the next available swapchain image:
    vk::ResultValue<uint32_t> current_buffer = m_device.acquireNextImageKHR(m_swapchain, m_fence_timeout, image_available_semaphore, nullptr);
    assert(current_buffer.result == vk::Result::eSuccess);
    assert(current_buffer.value < m_frame_buffers.size());

    command_buffer.begin(vk::CommandBufferBeginInfo(vk::CommandBufferUsageFlags()));

    std::array<vk::ClearValue, 2> clear_values;
    clear_values[0].color        = vk::ClearColorValue(std::array<float, 4>({{0.2f, 0.2f, 0.2f, 0.2f}}));
    clear_values[1].depthStencil = vk::ClearDepthStencilValue(1.0f, 0);

    auto&& r        = get_window_rect();
    auto&& extent2d = vk::Extent2D(r.Width(), r.Height());

    vk::RenderPassBeginInfo renderPassBeginInfo(m_render_pass, m_frame_buffers[current_buffer.value], vk::Rect2D(vk::Offset2D(0, 0), extent2d), clear_values);
    command_buffer.beginRenderPass(renderPassBeginInfo, vk::SubpassContents::eInline);
    command_buffer.bindPipeline(vk::PipelineBindPoint::eGraphics, m_pipeline);
    // m_command_buffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, pipelineLayout, 0, descriptorSet, nullptr);

    // m_command_buffer.bindVertexBuffers(0, vertexBufferData.buffer, {0});
    command_buffer.setViewport(0, vk::Viewport(0.0f, 0.0f, static_cast<float>(extent2d.width), static_cast<float>(extent2d.height), 0.0f, 1.0f));
    command_buffer.setScissor(0, vk::Rect2D(vk::Offset2D(0, 0), extent2d));

    // m_command_buffer.draw(12 * 3, 1, 0, 0);
    command_buffer.draw(3, 1, 0, 0);
    command_buffer.endRenderPass();
    command_buffer.end();

    vk::PipelineStageFlags wait_destination_stage_mask(vk::PipelineStageFlagBits::eColorAttachmentOutput);
    vk::SubmitInfo         submit_info(image_available_semaphore, wait_destination_stage_mask, command_buffer);

    vk::Queue graphics_queue = m_device.getQueue(m_graphics_queue_family_index, 0);
    graphics_queue.submit(submit_info, inflight_fence);

    while (vk::Result::eTimeout == m_device.waitForFences(inflight_fence, VK_TRUE, m_fence_timeout)) {
    };

    vk::Queue  present_queue = m_device.getQueue(m_present_queue_family_index, 0);
    vk::Result result        = present_queue.presentKHR(vk::PresentInfoKHR({}, m_swapchain, current_buffer.value));
    switch (result) {
    case vk::Result::eSuccess:
        break;
    case vk::Result::eSuboptimalKHR:
        std::cout << "vk::Queue::presentKHR returned vk::Result::eSuboptimalKHR !\n";
        break;
    default:
        assert(false); // an unexpected result is returned !
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    m_device.waitIdle();
}

void Device::draw()
{
    auto&& frame_resource_idx = curr_frame_resource_idx();

    auto&& image_available_semaphore = m_frame_resource[frame_resource_idx].m_image_available_semaphore;
    auto&& render_finished_semaphore = m_frame_resource[frame_resource_idx].m_render_finished_semaphore;
    auto&& inflight_fence            = m_frame_resource[frame_resource_idx].m_inflight_fence;

    auto&& command_buffer = m_frame_resource[frame_resource_idx].m_command_buffer;

    // UINT64_MAX, which effectively disables the timeout.
    auto&& result = m_device.waitForFences(inflight_fence, VK_TRUE, UINT64_MAX);
    m_device.resetFences(inflight_fence);

    // workaround for eErrorOutOfDateKHR
    // https://github.com/KhronosGroup/Vulkan-Hpp/issues/599
    uint32_t buffer_id = 0;
    result             = m_device.acquireNextImageKHR(m_swapchain, m_fence_timeout, image_available_semaphore, nullptr, &buffer_id);
    if (result == vk::Result::eErrorOutOfDateKHR) {
        // recreate swapchain
        return;
    }
    else if (result != vk::Result::eSuccess) {
        throw std::runtime_error("failed to acquire swap chain image!");
    }

    assert(buffer_id < m_frame_buffers.size());

    command_buffer.reset();
    command_buffer.begin(vk::CommandBufferBeginInfo(vk::CommandBufferUsageFlags()));

    std::array<vk::ClearValue, 2> clear_values;
    clear_values[0].color        = vk::ClearColorValue(std::array<float, 4>({{0.2f, 0.2f, 0.2f, 0.2f}}));
    clear_values[1].depthStencil = vk::ClearDepthStencilValue(1.0f, 0);

    vk::RenderPassBeginInfo renderPassBeginInfo(
        m_render_pass, m_frame_buffers[buffer_id], vk::Rect2D(vk::Offset2D(0, 0), m_swapchain_image_size), clear_values);
    command_buffer.beginRenderPass(renderPassBeginInfo, vk::SubpassContents::eInline);
    command_buffer.bindPipeline(vk::PipelineBindPoint::eGraphics, m_pipeline);
    // m_command_buffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, pipelineLayout, 0, descriptorSet, nullptr);

    // m_command_buffer.bindVertexBuffers(0, vertexBufferData.buffer, {0});
    command_buffer.setViewport(
        0, vk::Viewport(0.0f, 0.0f, static_cast<float>(m_swapchain_image_size.width), static_cast<float>(m_swapchain_image_size.height), 0.0f, 1.0f));
    command_buffer.setScissor(0, vk::Rect2D(vk::Offset2D(0, 0), m_swapchain_image_size));

    // m_command_buffer.draw(12 * 3, 1, 0, 0);
    command_buffer.draw(3, 1, 0, 0);
    command_buffer.endRenderPass();
    command_buffer.end();

    vk::PipelineStageFlags wait_destination_stage_mask(vk::PipelineStageFlagBits::eColorAttachmentOutput);
    vk::SubmitInfo         submit_info(image_available_semaphore, wait_destination_stage_mask, command_buffer, render_finished_semaphore);

    vk::Queue graphics_queue = m_device.getQueue(m_graphics_queue_family_index, 0);
    graphics_queue.submit(submit_info, inflight_fence);

    vk::Queue present_queue = m_device.getQueue(m_present_queue_family_index, 0);

    // workaround for eErrorOutOfDateKHR throw exception when Quiting
    // https://github.com/KhronosGroup/Vulkan-Hpp/issues/599
    auto&& present_info   = vk::PresentInfoKHR(render_finished_semaphore, m_swapchain, buffer_id);
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

std::vector<const char*> Device::gather_extensions(const std::vector<std::string>& extensions, const std::vector<vk::ExtensionProperties>& extension_properties)
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
    vk::DebugUtilsMessageSeverityFlagsEXT severityFlags(vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning | vk::DebugUtilsMessageSeverityFlagBitsEXT::eError);
    vk::DebugUtilsMessageTypeFlagsEXT     messageTypeFlags(vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral | vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance |
                                                           vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation);
    vk::StructureChain<vk::InstanceCreateInfo, vk::DebugUtilsMessengerCreateInfoEXT> instance_create_info(
        {{}, &application_info, layers, extensions}, {{}, severityFlags, messageTypeFlags, &debug_utils_messenger_callback});

    return instance_create_info;
}

vk::DebugUtilsMessengerEXT Device::create_debug_utils_messenger_EXT(const vk::Instance& instance)
{
    return instance.createDebugUtilsMessengerEXT(make_debug_utils_messenger_create_info_EXT());
}

vk::DebugUtilsMessengerCreateInfoEXT Device::make_debug_utils_messenger_create_info_EXT()
{
    return {{}, vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning | vk::DebugUtilsMessageSeverityFlagBitsEXT::eError,
        vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral | vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation,
        &debug_utils_messenger_callback};
}

VKAPI_ATTR VkBool32 VKAPI_CALL Device::debug_utils_messenger_callback(VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
    VkDebugUtilsMessageTypeFlagsEXT message_types, const VkDebugUtilsMessengerCallbackDataEXT* p_callback_data, void*)
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
    std::vector<std::string> extensions;
    extensions.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
#if defined(VK_USE_PLATFORM_WIN32_KHR)
    extensions.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
#endif
    return extensions;
}

std::vector<std::string> Device::get_device_extensions()
{
    return {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
}

vk::SurfaceFormatKHR Device::pick_surface_format(std::vector<vk::SurfaceFormatKHR> const& formats)
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

uint32_t Device::curr_frame_resource_idx()
{
    return m_frame_count % MAX_FRAMES_IN_FLIGHT;
}

void Device::create_pipeline_state_object()
{
    auto&& device = m_device;

    // Render pass
    auto&& render_pass = m_render_pass;

    // Programable state -----------
    std::array<vk::PipelineShaderStageCreateInfo, 2> pipeline_shader_stage_createinfo = {
        vk::PipelineShaderStageCreateInfo(vk::PipelineShaderStageCreateFlags(), vk::ShaderStageFlagBits::eVertex, m_vertex_shader.m_shader_module, "main"),
        vk::PipelineShaderStageCreateInfo(vk::PipelineShaderStageCreateFlags(), vk::ShaderStageFlagBits::eFragment, m_pixel_shader.m_shader_module, "main")};

    // vk::VertexInputBindingDescription                  vertexInputBindingDescription(0, sizeof(coloredCubeData[0]));
    // std::array<vk::VertexInputAttributeDescription, 2> vertexInputAttributeDescriptions = {
    //     vk::VertexInputAttributeDescription(0, 0, vk::Format::eR32G32B32A32Sfloat, 0),
    //     vk::VertexInputAttributeDescription(1, 0, vk::Format::eR32G32B32A32Sfloat, 16)};
    // vk::PipelineVertexInputStateCreateInfo pipelineVertexInputStateCreateInfo(vk::PipelineVertexInputStateCreateFlags(), // flags
    //     vertexInputBindingDescription,                                                                                   // vertexBindingDescriptions
    //     vertexInputAttributeDescriptions                                                                                 // vertexAttributeDescriptions
    //);

    // blank layout, no shader resources
    auto&& pipeline_layout = m_vertex_shader.m_pipeline_layout;
    pipeline_layout        = device.createPipelineLayout(vk::PipelineLayoutCreateInfo(vk::PipelineLayoutCreateFlags()));

    // vk::PipelineVertexInputStateCreateInfo pipeline_vertex_input_state_createinfo(vk::PipelineVertexInputStateCreateFlags(), // flags
    //     vertexInputBindingDescription,                                                                                       // vertexBindingDescriptions
    //     vertexInputAttributeDescriptions                                                                                     // vertexAttributeDescriptions
    //);

    vk::PipelineVertexInputStateCreateInfo pipeline_vertex_input_state_createinfo(vk::PipelineVertexInputStateCreateFlags(), // flags
        0, nullptr, 0, nullptr);

    // ----------

    // Fixed pipeline state -----------
    vk::PipelineInputAssemblyStateCreateInfo pipeline_input_assembly_state_createinfo(
        vk::PipelineInputAssemblyStateCreateFlags(), vk::PrimitiveTopology::eTriangleList);

    vk::PipelineViewportStateCreateInfo pipeline_viewport_state_createinfo(vk::PipelineViewportStateCreateFlags(), 1, nullptr, 1, nullptr);

    vk::PipelineRasterizationStateCreateInfo pipeline_rasterization_state_createinfo(vk::PipelineRasterizationStateCreateFlags(), // flags
        false,                                                                                                                    // depthClampEnable
        false,                                                                                                                    // rasterizerDiscardEnable
        vk::PolygonMode::eFill,                                                                                                   // polygonMode
        vk::CullModeFlagBits::eBack,                                                                                              // cullMode
        vk::FrontFace::eClockwise,                                                                                                // frontFace
        false,                                                                                                                    // depthBiasEnable
        0.0f,                                                                                                                     // depthBiasConstantFactor
        0.0f,                                                                                                                     // depthBiasClamp
        0.0f,                                                                                                                     // depthBiasSlopeFactor
        1.0f                                                                                                                      // lineWidth
    );

    vk::PipelineMultisampleStateCreateInfo pipeline_multisample_state_createinfo(vk::PipelineMultisampleStateCreateFlags(), // flags
        vk::SampleCountFlagBits::e1                                                                                         // rasterizationSamples
                                                                                                                            // other values can be default
    );

    vk::StencilOpState                      stencil_op_state(vk::StencilOp::eKeep, vk::StencilOp::eKeep, vk::StencilOp::eKeep, vk::CompareOp::eAlways);
    vk::PipelineDepthStencilStateCreateInfo pipeline_depth_stencil_state_createinfo(vk::PipelineDepthStencilStateCreateFlags(), // flags
        true,                                                                                                                   // depthTestEnable
        true,                                                                                                                   // depthWriteEnable
        vk::CompareOp::eLessOrEqual,                                                                                            // depthCompareOp
        false,                                                                                                                  // depthBoundTestEnable
        false,                                                                                                                  // stencilTestEnable
        stencil_op_state,                                                                                                       // front
        stencil_op_state                                                                                                        // back
    );

    vk::ColorComponentFlags color_component_flags(
        vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA);
    vk::PipelineColorBlendAttachmentState pipeline_color_blend_attachment_state(false, // blendEnable
        vk::BlendFactor::eZero,                                                        // srcColorBlendFactor
        vk::BlendFactor::eZero,                                                        // dstColorBlendFactor
        vk::BlendOp::eAdd,                                                             // colorBlendOp
        vk::BlendFactor::eZero,                                                        // srcAlphaBlendFactor
        vk::BlendFactor::eZero,                                                        // dstAlphaBlendFactor
        vk::BlendOp::eAdd,                                                             // alphaBlendOp
        color_component_flags                                                          // colorWriteMask
    );

    vk::PipelineColorBlendStateCreateInfo pipeline_color_blend_state_createinfo(vk::PipelineColorBlendStateCreateFlags(), // flags
        false,                                                                                                            // logicOpEnable
        vk::LogicOp::eNoOp,                                                                                               // logicOp
        pipeline_color_blend_attachment_state,                                                                            // attachments
        {{1.0f, 1.0f, 1.0f, 1.0f}}                                                                                        // blendConstants
    );

    std::array<vk::DynamicState, 2>    dynamic_states = {vk::DynamicState::eViewport, vk::DynamicState::eScissor};
    vk::PipelineDynamicStateCreateInfo pipeline_dynamic_state_createinfo(vk::PipelineDynamicStateCreateFlags(), dynamic_states);
    // -----------

    vk::GraphicsPipelineCreateInfo graphics_pipeline_createinfo(vk::PipelineCreateFlags(), // flags
        pipeline_shader_stage_createinfo,                                                  // stages
        &pipeline_vertex_input_state_createinfo,                                           // pVertexInputState
        &pipeline_input_assembly_state_createinfo,                                         // pInputAssemblyState
        nullptr,                                                                           // pTessellationState
        &pipeline_viewport_state_createinfo,                                               // pViewportState
        &pipeline_rasterization_state_createinfo,                                          // pRasterizationState
        &pipeline_multisample_state_createinfo,                                            // pMultisampleState
        &pipeline_depth_stencil_state_createinfo,                                          // pDepthStencilState
        &pipeline_color_blend_state_createinfo,                                            // pColorBlendState
        &pipeline_dynamic_state_createinfo,                                                // pDynamicState
        pipeline_layout,                                                                   // layout
        render_pass                                                                        // renderPass
    );

    vk::Result   result;
    vk::Pipeline pipeline;
    std::tie(result, pipeline) = device.createGraphicsPipeline(nullptr, graphics_pipeline_createinfo);
    switch (result) {
    case vk::Result::eSuccess:
        break;
    case vk::Result::ePipelineCompileRequiredEXT:
        // something meaningfull here
        break;
    default:
        assert(false); // should never happen
    }

    m_pipeline = pipeline;
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

} // namespace VKN
