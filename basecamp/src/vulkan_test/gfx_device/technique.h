#pragma once

namespace VKN {

    class Device;

    struct Descriptorset_layoutdata;

    struct Technique_desc {
        std::string m_vs;
        std::string m_ps;
    };

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

        std::vector<vk::DescriptorSetLayout>   m_descriptorset_layouts;
        std::vector<Descriptorset_layoutdata*> m_descriptorset_infos;

        vk::PipelineLayout m_pipeline_layout;
        vk::Pipeline       m_pipeline;
    };

} // namespace VKN
