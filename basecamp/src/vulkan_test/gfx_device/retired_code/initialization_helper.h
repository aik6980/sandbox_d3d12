#pragma once

/// <summary>
/// Since I decided to use vkBootStrap, so this class is here largely for a legacy reason
/// At some points this should be fully deprecated
/// </summary>

namespace Retired_code {
    class Initialization_helper {
      public:
        // output
        vk::Instance m_vk_instance;

        vk::PhysicalDevice m_physical_device;
        vk::Device m_device;

        vk::SwapchainKHR m_swapchain;
        std::vector<vk::ImageView> m_swapchain_image_views;
        std::vector<vk::Image> m_swapchain_images;
        vk::Extent2D m_swapchain_image_size;
        vk::Format m_swapchain_format;

        vk::Queue m_graphics_queue;
        vk::Queue m_present_queue;
        uint32_t m_graphics_queue_family_index = 0;
        uint32_t m_present_queue_family_index  = 0;
        // input
        uint32_t m_req_api_version;
        std::vector<const char*> m_instance_extension_names;

        std::vector<const char*> m_physical_device_extension_names;

        vk::SurfaceKHR m_surface;

      public:
        void create_instance();
        void create_device();
        void create_swapchain(const CRect& win_rect);

        // document my legacy destroy routine before moving to vk-bootstrap
        void destroy();

        bool is_instance_extension_enabled(const std::string& name);

        std::vector<const char*> gather_layers(
            const std::vector<std::string>& layers, const std::vector<vk::LayerProperties>& layer_properties);
        std::vector<const char*> gather_extensions(
            const std::vector<const char*>& extensions, const std::vector<vk::ExtensionProperties>& extension_properties);

#if defined(DEBUG)
        vk::StructureChain<vk::InstanceCreateInfo, vk::DebugUtilsMessengerCreateInfoEXT> make_instance_create_info_chain(
            const vk::ApplicationInfo& application_info,
            const std::vector<const char*>& layers,
            const std::vector<const char*>& extensions);
#else
        vk::StructureChain<vk::InstanceCreateInfo> make_instance_create_info_chain(
            const vk::ApplicationInfo& application_info,
            const std::vector<const char*>& layers,
            const std::vector<const char*>& extensions);
#endif

        vk::DebugUtilsMessengerEXT create_debug_utils_messenger_EXT(const vk::Instance& instance);
        vk::DebugUtilsMessengerCreateInfoEXT make_debug_utils_messenger_create_info_EXT();

        static VKAPI_ATTR VkBool32 VKAPI_CALL debug_utils_messenger_callback(
            vk::DebugUtilsMessageSeverityFlagBitsEXT message_severity,
            vk::DebugUtilsMessageTypeFlagsEXT message_types,
            const vk::DebugUtilsMessengerCallbackDataEXT* p_callback_data,
            void* p_user_data = nullptr);

        template <typename Structure_type>
        Structure_type& request_extension_features();

        uint32_t find_graphics_queue_family_index(const std::vector<vk::QueueFamilyProperties>& queue_family_properties);
        uint32_t find_present_queue_family_index(uint32_t graphics_queue_family_index);

        // extension features
        // Holds the extension feature structures, we use a map to retain an order of requested structures
        std::unordered_map<vk::StructureType, std::shared_ptr<void>> m_extension_features;
        // The extension feature pointer
        void* m_last_requested_extension_feature{nullptr};

        vk::DebugUtilsMessengerEXT m_debug_utils_messenger;
    };

    /**
     * @brief Requests a third party extension to be used by the framework
     *
     *        To have the features enabled, this function must be called before the logical device
     *        is created. To do this request sample specific features inside
     *        VulkanSample::request_gpu_features(vkb::HPPPhysicalDevice &gpu).
     *
     *        If the feature extension requires you to ask for certain features to be enabled, you can
     *        modify the struct returned by this function, it will propegate the changes to the logical
     *        device.
     * @returns The extension feature struct
     */
    template <typename Structure_type>
    Structure_type& Initialization_helper::request_extension_features()
    {
        // We cannot request extension features if the physical device properties 2 instance extension isnt enabled
        if (!is_instance_extension_enabled(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME)) {
            throw std::runtime_error("Couldn't request feature from device as " +
                                     std::string(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME) +
                                     " isn't enabled!");
        }

        // If the type already exists in the map, return a casted pointer to get the extension feature struct
        vk::StructureType structure_type =
            Structure_type::structureType; // need to instantiate this value to be usable in find()!
        auto extension_features_it = m_extension_features.find(structure_type);
        if (extension_features_it != m_extension_features.end()) {
            return *static_cast<Structure_type*>(extension_features_it->second.get());
        }

        // Get the extension feature
        vk::StructureChain<vk::PhysicalDeviceFeatures2KHR, Structure_type> feature_chain =
            m_physical_device.getFeatures2KHR<vk::PhysicalDeviceFeatures2KHR, Structure_type>();

        // Insert the extension feature into the extension feature map so its ownership is held
        m_extension_features.insert(
            {structure_type, std::make_shared<Structure_type>(feature_chain.template get<Structure_type>())});

        // Pull out the dereferenced void pointer, we can assume its type based on the template
        auto* extension_ptr = static_cast<Structure_type*>(m_extension_features.find(structure_type)->second.get());

        // If an extension feature has already been requested, we shift the linked list down by one
        // Making this current extension the new base pointer
        if (m_last_requested_extension_feature) {
            extension_ptr->pNext = m_last_requested_extension_feature;
        }
        m_last_requested_extension_feature = extension_ptr;

        return *extension_ptr;
    }
} // namespace Retired_code
