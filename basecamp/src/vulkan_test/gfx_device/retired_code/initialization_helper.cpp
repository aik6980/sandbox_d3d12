#include "initialization_helper.h"

#include "common/common_cpp.h"

namespace Retired_code {

    void Initialization_helper::create_instance()
    {
        // init paramaters
        const auto& app_name    = std::string("vulkan_gfx");
        const auto& api_version = m_req_api_version;
        const auto& layers      = std::vector<std::string>{};
        const auto& extensions  = m_instance_extension_names;

#if (VULKAN_HPP_DISPATCH_LOADER_DYNAMIC == 1)
        static vk::detail::DynamicLoader dl;
        PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr =
            dl.getProcAddress<PFN_vkGetInstanceProcAddr>("vkGetInstanceProcAddr");
        VULKAN_HPP_DEFAULT_DISPATCHER.init(vkGetInstanceProcAddr);
#endif

        vk::ApplicationInfo applicationInfo{.pApplicationName = app_name.c_str(),
            .applicationVersion                               = 1,
            .pEngineName                                      = app_name.c_str(),
            .engineVersion                                    = 1,
            .apiVersion                                       = api_version};

        std::vector<const char*> enabled_layers = gather_layers(layers, vk::enumerateInstanceLayerProperties());
        std::vector<const char*> enabled_extensions =
            gather_extensions(extensions, vk::enumerateInstanceExtensionProperties());

        // create instance
        m_vk_instance =
            vk::createInstance(make_instance_create_info_chain(applicationInfo, enabled_layers, enabled_extensions)
                    .get<vk::InstanceCreateInfo>());

#if (VULKAN_HPP_DISPATCH_LOADER_DYNAMIC == 1)
        // initialize function pointers for instance
        VULKAN_HPP_DEFAULT_DISPATCHER.init(m_vk_instance);
#endif

#if defined(_DEBUG)
        // create debug layer
        m_debug_utils_messenger = create_debug_utils_messenger_EXT(m_vk_instance);
#endif
    }

    std::vector<const char*> Initialization_helper::gather_layers(
        const std::vector<std::string>& layers, const std::vector<vk::LayerProperties>& layer_properties)
    {
        std::vector<char const*> enabled_layers;
        enabled_layers.reserve(layers.size());

        for (auto&& layer : layers) {
            assert(std::find_if(layer_properties.begin(), layer_properties.end(), [layer](const vk::LayerProperties& lp) {
                return layer == lp.layerName;
            }) != layer_properties.end());
            enabled_layers.push_back(layer.data());
        }

        // Enable standard validation layer to find as much errors as possible!
        if (std::find(layers.begin(), layers.end(), "VK_LAYER_KHRONOS_validation") == layers.end() &&
            std::find_if(layer_properties.begin(), layer_properties.end(), [](const vk::LayerProperties& lp) {
                return (strcmp("VK_LAYER_KHRONOS_validation", lp.layerName) == 0);
            }) != layer_properties.end()) {
            enabled_layers.push_back("VK_LAYER_KHRONOS_validation");
        }

        return enabled_layers;
    }

    std::vector<const char*> Initialization_helper::gather_extensions(
        const std::vector<const char*>& extensions, const std::vector<vk::ExtensionProperties>& extension_properties)
    {
        std::vector<char const*> enabled_extensions;
        enabled_extensions.reserve(extensions.size());
        for (const auto& ext : extensions) {
            assert(std::find_if(
                       extension_properties.begin(), extension_properties.end(), [ext](const vk::ExtensionProperties& ep) {
                           return (strcmp(ext, ep.extensionName) == 0);
                       }) != extension_properties.end());
            enabled_extensions.push_back(ext);
        }
#if defined(_DEBUG)
        if (std::find(extensions.begin(), extensions.end(), VK_EXT_DEBUG_UTILS_EXTENSION_NAME) == extensions.end() &&
            std::find_if(extension_properties.begin(), extension_properties.end(), [](const vk::ExtensionProperties& ep) {
                return (strcmp(VK_EXT_DEBUG_UTILS_EXTENSION_NAME, ep.extensionName) == 0);
            }) != extension_properties.end()) {
            enabled_extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        }
#endif
        return enabled_extensions;
    }

#if defined(_DEBUG)
    vk::StructureChain<vk::InstanceCreateInfo, vk::DebugUtilsMessengerCreateInfoEXT>
    Initialization_helper::make_instance_create_info_chain(const vk::ApplicationInfo& application_info,
        const std::vector<const char*>& layers,
        const std::vector<const char*>& extensions)
    {
        // in debug mode, addionally use the debugUtilsMessengerCallback in instance creation!
        vk::DebugUtilsMessageSeverityFlagsEXT severityFlags(
            vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning | vk::DebugUtilsMessageSeverityFlagBitsEXT::eError);
        vk::DebugUtilsMessageTypeFlagsEXT messageTypeFlags(vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
                                                           vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance |
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
#else
    vk::StructureChain<vk::InstanceCreateInfo> Initialization_helper::make_instance_create_info_chain(
        const vk::ApplicationInfo& application_info,
        const std::vector<const char*>& layers,
        const std::vector<const char*>& extensions)
    {
        // in non-debug mode just use the InstanceCreateInfo for instance creation
        vk::InstanceCreateInfo instance_createinfo{
            .pApplicationInfo        = &application_info,
            .enabledLayerCount       = (uint32_t)layers.size(),
            .ppEnabledLayerNames     = layers.data(),
            .enabledExtensionCount   = (uint32_t)extensions.size(),
            .ppEnabledExtensionNames = extensions.data(),
        };

        vk::StructureChain<vk::InstanceCreateInfo> complete_instance_create_info(instance_createinfo);
        return complete_instance_create_info;
    }
#endif

    vk::DebugUtilsMessengerEXT Initialization_helper::create_debug_utils_messenger_EXT(const vk::Instance& instance)
    {
        return instance.createDebugUtilsMessengerEXT(make_debug_utils_messenger_create_info_EXT());
    }

    vk::DebugUtilsMessengerCreateInfoEXT Initialization_helper::make_debug_utils_messenger_create_info_EXT()
    {
        vk::DebugUtilsMessageSeverityFlagsEXT severityFlags(
            vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning | vk::DebugUtilsMessageSeverityFlagBitsEXT::eError);
        vk::DebugUtilsMessageTypeFlagsEXT messageTypeFlags(vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
                                                           vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance |
                                                           vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation);

        return vk::DebugUtilsMessengerCreateInfoEXT{.flags = {},
            .messageSeverity                               = severityFlags,
            .messageType                                   = messageTypeFlags,
            .pfnUserCallback                               = debug_utils_messenger_callback};
    }

    VKAPI_ATTR VkBool32 VKAPI_CALL Initialization_helper::debug_utils_messenger_callback(
        vk::DebugUtilsMessageSeverityFlagBitsEXT message_severity,
        vk::DebugUtilsMessageTypeFlagsEXT message_types,
        const vk::DebugUtilsMessengerCallbackDataEXT* p_callback_data,
        void*)
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
                          << "objectType   = "
                          << vk::to_string(static_cast<vk::ObjectType>(p_callback_data->pObjects[i].objectType)) << "\n";
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

    void Initialization_helper::create_device()
    {
        // enumerate the physical devices
        m_physical_device = m_vk_instance.enumeratePhysicalDevices().front();

        // get the QueueFamilyProperties of the first PhysicalDevice
        std::vector<vk::QueueFamilyProperties> queue_family_properties = m_physical_device.getQueueFamilyProperties();

        auto&& graphics_queue_family_index = find_graphics_queue_family_index(queue_family_properties);
        assert(graphics_queue_family_index < queue_family_properties.size());
        m_graphics_queue_family_index = static_cast<uint32_t>(graphics_queue_family_index);
        // find Present queue family index
        auto&& present_queue_family_index = find_present_queue_family_index(graphics_queue_family_index);
        m_present_queue_family_index      = static_cast<uint32_t>(present_queue_family_index);

        // find device extension
        auto&& device_extensions = m_physical_device_extension_names;

        // enabling extension features
        auto& requested_dynamic_rendering = request_extension_features<vk::PhysicalDeviceDynamicRenderingFeaturesKHR>();
        requested_dynamic_rendering.dynamicRendering = VK_TRUE;

        auto&& requested_device_synchronization2 =
            request_extension_features<vk::PhysicalDeviceSynchronization2FeaturesKHR>();
        requested_device_synchronization2.synchronization2 = VK_TRUE;
        //

        std::vector<const char*> enabled_extensions;
        enabled_extensions.reserve(device_extensions.size());
        for (auto&& ext : device_extensions) {
            enabled_extensions.push_back(ext);
        }

        // create a Device
        float queue_priority = 0.0f;
        vk::DeviceQueueCreateInfo device_queue_createinfo{.flags = vk::DeviceQueueCreateFlags(),
            .queueFamilyIndex                                    = graphics_queue_family_index,
            .queueCount                                          = 1,
            .pQueuePriorities                                    = &queue_priority};

        vk::DeviceCreateInfo device_createinfo{
            .pNext                   = m_last_requested_extension_feature,
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
        // get queues
        m_graphics_queue = m_device.getQueue(static_cast<uint32_t>(graphics_queue_family_index), 0);
        m_present_queue  = m_device.getQueue(static_cast<uint32_t>(present_queue_family_index), 0);
    }

    void Initialization_helper::create_swapchain(const CRect& win_rect)
    {
        auto&& rect = win_rect;
        // get the supported VkFormats
        std::vector<vk::SurfaceFormatKHR> formats = m_physical_device.getSurfaceFormatsKHR(m_surface);
        assert(!formats.empty());
        vk::Format format = (formats[0].format == vk::Format::eUndefined) ? vk::Format::eB8G8R8A8Unorm : formats[0].format;

        vk::SurfaceCapabilitiesKHR surface_capabilities = m_physical_device.getSurfaceCapabilitiesKHR(m_surface);
        vk::Extent2D swapchain_extent;
        if (surface_capabilities.currentExtent.width == std::numeric_limits<uint32_t>::max()) {
            // If the surface size is undefined, the size is set to the size of the images
            // requested.
            swapchain_extent.width  = std::clamp((uint32_t)rect.Width(),
                surface_capabilities.minImageExtent.width,
                surface_capabilities.maxImageExtent.width);
            swapchain_extent.height = std::clamp((uint32_t)rect.Height(),
                surface_capabilities.minImageExtent.height,
                surface_capabilities.maxImageExtent.height);
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

        vk::SurfaceTransformFlagBitsKHR pre_transform =
            (surface_capabilities.supportedTransforms & vk::SurfaceTransformFlagBitsKHR::eIdentity)
                ? vk::SurfaceTransformFlagBitsKHR::eIdentity
                : surface_capabilities.currentTransform;

        vk::CompositeAlphaFlagBitsKHR composite_alpha =
            (surface_capabilities.supportedCompositeAlpha & vk::CompositeAlphaFlagBitsKHR::ePreMultiplied)
                ? vk::CompositeAlphaFlagBitsKHR::ePreMultiplied
            : (surface_capabilities.supportedCompositeAlpha & vk::CompositeAlphaFlagBitsKHR::ePostMultiplied)
                ? vk::CompositeAlphaFlagBitsKHR::ePostMultiplied
            : (surface_capabilities.supportedCompositeAlpha & vk::CompositeAlphaFlagBitsKHR::eInherit)
                ? vk::CompositeAlphaFlagBitsKHR::eInherit
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
            .imageSharingMode = vk::SharingMode::eExclusive, // Access mode of the images (exclusive
                                                             // to one queue family)
            .preTransform   = pre_transform,
            .compositeAlpha = composite_alpha,
            .presentMode    = swapchain_present_mode,
            .clipped        = true, // Whether to clip obscured pixels (improves performance)
            .oldSwapchain   = prev_swapchain,
        };

        // find Present queue family index
        // m_present_queue_family_index = find_present_queue_family_index();

        uint32_t queue_family_indices[2] = {m_graphics_queue_family_index, m_present_queue_family_index};
        if (m_graphics_queue_family_index != m_present_queue_family_index) {
            // If the graphics and present queues are from different queue families, we either have
            // to explicitly transfer ownership of images between the queues, or we have to create
            // the swapchain with imageSharingMode as VK_SHARING_MODE_CONCURRENT
            swapchain_create_info.imageSharingMode      = vk::SharingMode::eConcurrent;
            swapchain_create_info.queueFamilyIndexCount = 2;
            swapchain_create_info.pQueueFamilyIndices   = queue_family_indices;
        }

        m_swapchain = m_device.createSwapchainKHR(swapchain_create_info);

        m_swapchain_images = m_device.getSwapchainImagesKHR(m_swapchain);

        auto&& image_views = m_swapchain_image_views;
        image_views.reserve(m_swapchain_images.size());

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

        for (auto&& image : m_swapchain_images) {
            image_view_create_info.image = image;
            image_views.push_back(m_device.createImageView(image_view_create_info));
        }

        m_swapchain_format = format;
    }

    bool Initialization_helper::is_instance_extension_enabled(const std::string& name)
    {
        auto&& instance_extensions = m_instance_extension_names;
        return std::find_if(instance_extensions.begin(), instance_extensions.end(), [name](const char* enabled_extension) {
            return strcmp(name.c_str(), enabled_extension) == 0;
        }) != instance_extensions.end();
    }

    uint32_t Initialization_helper::find_graphics_queue_family_index(
        const std::vector<vk::QueueFamilyProperties>& queue_family_properties)
    {
        // get the first index into queueFamiliyProperties which supports graphics
        std::vector<vk::QueueFamilyProperties>::const_iterator graphics_queue_family_property = std::find_if(
            queue_family_properties.begin(), queue_family_properties.end(), [](vk::QueueFamilyProperties const& qfp) {
                return qfp.queueFlags & vk::QueueFlagBits::eGraphics;
            });
        assert(graphics_queue_family_property != queue_family_properties.end());
        return static_cast<uint32_t>(std::distance(queue_family_properties.begin(), graphics_queue_family_property));
    }

    uint32_t Initialization_helper::find_present_queue_family_index(uint32_t graphics_queue_family_index)
    {
        // get the QueueFamilyProperties of the first PhysicalDevice
        std::vector<vk::QueueFamilyProperties> queue_family_properties = m_physical_device.getQueueFamilyProperties();

        // determine a queueFamilyIndex that suports present
        // first check if the graphicsQueueFamiliyIndex is good enough
        uint32_t present_queue_family_index = m_physical_device.getSurfaceSupportKHR(graphics_queue_family_index, m_surface)
                                                  ? graphics_queue_family_index
                                                  : (uint32_t)queue_family_properties.size();
        if (present_queue_family_index == queue_family_properties.size()) {
            // the graphicsQueueFamilyIndex doesn't support present -> look for an other family
            // index that supports both graphics and present
            for (size_t i = 0; i < queue_family_properties.size(); i++) {
                if ((queue_family_properties[i].queueFlags & vk::QueueFlagBits::eGraphics) &&
                    m_physical_device.getSurfaceSupportKHR(static_cast<uint32_t>(i), m_surface)) {
                    graphics_queue_family_index = static_cast<uint32_t>(i);
                    present_queue_family_index  = static_cast<uint32_t>(i);
                    break;
                }
            }
            if (present_queue_family_index == queue_family_properties.size()) {
                // there's nothing like a single family index that supports both graphics and
                // present -> look for an other family index that supports present
                for (size_t i = 0; i < queue_family_properties.size(); i++) {
                    if (m_physical_device.getSurfaceSupportKHR(static_cast<uint32_t>(i), m_surface)) {
                        present_queue_family_index = static_cast<uint32_t>(i);

                        break;
                    }
                }
            }
        }
        if ((graphics_queue_family_index == queue_family_properties.size()) ||
            (present_queue_family_index == queue_family_properties.size())) {
            throw std::runtime_error("Could not find a queue for graphics or present -> terminating");
        }

        return present_queue_family_index;
    }

    void Initialization_helper::destroy()
    {
        DBG::OutputString("Documention only! Do not use in code");
        return;

        // wait until GPU finished all the work
        m_device.waitIdle();

        // destroy sync object
        // destroy_sync_object();

        // retired - destroy render pass
        // for (auto framebuffer : m_frame_buffers) {
        //    m_device.destroyFramebuffer(framebuffer);
        //}
        // m_device.destroyRenderPass(m_render_pass);

        // destroy shader
        // m_shader_manager->destroy_resources();

        // destroy resources
        // m_resource_manager->destroy();
        // destroy_resource(m_depth_buffer);

        // destroy the imageViews, the swapChain,and the surface
        for (auto& view : m_swapchain_image_views) {
            m_device.destroyImageView(view);
        }
        m_device.destroySwapchainKHR(m_swapchain);
        m_vk_instance.destroySurfaceKHR(m_surface);

        // destroy command buffer
        // freeing the commandBuffer is optional, as it will automatically freed when the
        // corresponding CommandPool is destroyed.
        // for (uint32_t i = 0; i < m_frame_resource.size(); ++i) {
        //    m_device.freeCommandBuffers(m_command_pool, m_frame_resource[i]->m_command_buffer);
        //    m_frame_resource[i]->destroy_resources();
        //}
        // destroy the command pool
        // m_device.destroyCommandPool(m_command_pool);

        // vma allocator
        // m_vma_allocator.destroy();

        // destroy the device
        m_device.destroy();

        // destroy instance
        m_vk_instance.destroyDebugUtilsMessengerEXT(m_debug_utils_messenger); // retired : move to vk-bootstrap
        m_vk_instance.destroy();
    }

    vk::SurfaceFormatKHR Initialization_helper::pick_surface_format(std::vector<vk::SurfaceFormatKHR> const& formats) const
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

} // namespace Retired_code
