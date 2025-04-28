#pragma once

#include "vma/vma.h"

#include "buffer.h"
#include "frame_resource.h"

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

		void create_instance();
		void create_device();
		void create_vma_allocator();

		void create_command_buffer();
		void create_swapchain();
		void create_depth_buffer();

		void create_render_pass();

		void create_sync_object();
		void destroy_sync_object();

		void create_descriptor_pool();

		void draw_once();
		void draw();

		static uint32_t format_size(vk::Format format);

		void load_resources();

		void begin_single_command_submission();
		void end_single_command_submission();

		void begin_frame();
		void end_frame();

		// Sub system
		std::unique_ptr<Resource_manager> m_resource_manager;
		std::unique_ptr<Shader_manager>	  m_shader_manager;

	  private:
		CRect get_window_rect() const;

		std::vector<const char*> gather_layers(const std::vector<std::string>& layers, const std::vector<vk::LayerProperties>& layer_properties);
		std::vector<const char*> gather_extensions(
			const std::vector<std::string>& extensions, const std::vector<vk::ExtensionProperties>& extension_properties);

		vk::StructureChain<vk::InstanceCreateInfo, vk::DebugUtilsMessengerCreateInfoEXT> make_instance_create_info_chain(
			const vk::ApplicationInfo& application_info, const std::vector<const char*>& layers, const std::vector<const char*>& extensions);

		vk::DebugUtilsMessengerEXT			  create_debug_utils_messenger_EXT(const vk::Instance& instance);
		vk::DebugUtilsMessengerCreateInfoEXT  make_debug_utils_messenger_create_info_EXT();
		static VKAPI_ATTR VkBool32 VKAPI_CALL debug_utils_messenger_callback(VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
			VkDebugUtilsMessageTypeFlagsEXT message_types, const VkDebugUtilsMessengerCallbackDataEXT* p_callback_data, void* /*p_user_data*/);

		uint32_t find_graphics_queue_family_index(const std::vector<vk::QueueFamilyProperties>& queue_family_properties);
		uint32_t find_present_queue_family_index();

		std::vector<std::string> get_instance_extensions();
		bool					 is_instance_extension_enabled(const std::string& name);
		std::vector<std::string> get_device_extensions();

		vk::SurfaceFormatKHR pick_surface_format(std::vector<vk::SurfaceFormatKHR> const& formats);

		uint32_t		curr_frame_resource_idx();
		Frame_resource& curr_frame_resource();

		vk::CommandBuffer* curr_command_buffer();

		template <typename Structure_type>
		Structure_type& request_extension_features();

		// single
		vk::CommandBuffer m_single_use_command_buffer;

		// Resource manager
		void destroy_resource(Image& resource);

		// Window
		HINSTANCE m_hinstance;
		HWND	  m_hwnd;

		// Instance
		static const uint32_t	   m_req_api_version = VK_API_VERSION_1_3;
		vk::Instance			   m_instance;
		vk::DebugUtilsMessengerEXT m_debug_utils_messenger;

		vk::PhysicalDevice m_physical_device;

		// extension features
		// Holds the extension feature structures, we use a map to retain an order of requested structures
		std::unordered_map<vk::StructureType, std::shared_ptr<void>> m_extension_features;
		// The extension feature pointer
		void* m_last_requested_extension_feature{nullptr};

		vk::Device m_device;
		uint32_t   m_graphics_queue_family_index = 0;
		uint32_t   m_present_queue_family_index	 = 0;

		//
		const static uint32_t MAX_FRAMES_IN_FLIGHT	 = 2;
		uint32_t			  m_frame_count			 = 0;
		uint32_t			  m_swapchain_buffer_idx = 0;

		// Command pool
		vk::CommandPool m_command_pool;
		// vk::CommandBuffer m_command_buffer; // move into Frame_resource

		// Window surface
		vk::SurfaceKHR			   m_surface;
		std::vector<vk::ImageView> m_swapchain_image_views;
		vk::SwapchainKHR		   m_swapchain;
		vk::Extent2D			   m_swapchain_image_size;
		vk::Format				   m_swapchain_format;

		// VMA Allocator
		vma::Allocator m_vma_allocator;

		// Buffer
		Image m_depth_buffer;

		// Render pass/Frame buffer (for swapchain)
		vk::RenderPass				 m_render_pass;
		std::vector<vk::Framebuffer> m_frame_buffers;

		// Syncronization
		const uint64_t m_fence_timeout = 100000000;

		std::array<std::unique_ptr<Frame_resource>, MAX_FRAMES_IN_FLIGHT> m_frame_resource;
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
	Structure_type& Device::request_extension_features()
	{
		// We cannot request extension features if the physical device properties 2 instance extension isnt enabled
		if (!is_instance_extension_enabled(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME)) {
			throw std::runtime_error(
				"Couldn't request feature from device as " + std::string(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME) + " isn't enabled!");
		}

		// If the type already exists in the map, return a casted pointer to get the extension feature struct
		vk::StructureType structure_type		= Structure_type::structureType; // need to instantiate this value to be usable in find()!
		auto			  extension_features_it = m_extension_features.find(structure_type);
		if (extension_features_it != m_extension_features.end()) {
			return *static_cast<Structure_type*>(extension_features_it->second.get());
		}

		// Get the extension feature
		vk::StructureChain<vk::PhysicalDeviceFeatures2KHR, Structure_type> feature_chain =
			m_physical_device.getFeatures2KHR<vk::PhysicalDeviceFeatures2KHR, Structure_type>();

		// Insert the extension feature into the extension feature map so its ownership is held
		m_extension_features.insert({structure_type, std::make_shared<Structure_type>(feature_chain.template get<Structure_type>())});

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

} // namespace VKN
