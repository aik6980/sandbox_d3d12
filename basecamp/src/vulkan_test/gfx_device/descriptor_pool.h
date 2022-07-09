#pragma once

namespace VKN {

    class Device;
    class Descriptor_pool {
      public:
        Descriptor_pool(Device& gfx_device)
            : m_gfx_device(gfx_device)
        {
        }

        void create_pool();
        void destroy_resources();

        void reset();

        vk::DescriptorSet& create_descriptor_set(const vk::DescriptorSetLayout& layout);

        vk::DescriptorPool             m_descriptor_pool;
        std::vector<vk::DescriptorSet> m_descriptor_sets;

        static const uint32_t m_max_descriptor = 256;

      private:
        Device& m_gfx_device;
    };
} // namespace VKN
