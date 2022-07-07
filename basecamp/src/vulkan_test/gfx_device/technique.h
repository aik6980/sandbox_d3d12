#pragma once

namespace VKN {

    class Device;

    class Technique {

      public:
        Technique(Device& gfx_device)
            : m_gfx_device(gfx_device)
        {
        }

        void destroy();

        void create_pipeline();

      public:
        Device& m_gfx_device;

        std::vector<vk::DescriptorSetLayout> m_decriptorset_layouts;
        vk::PipelineLayout                   m_pipeline_layout;
        vk::Pipeline                         m_pipeline;
    };

} // namespace VKN
