#pragma once

namespace VKN {

    class Device;
    class Shader;

    struct Descriptorset_layoutdata;

    struct Technique_createinfo {
        std::string m_vs_name;
        std::string m_ps_name;
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

        std::weak_ptr<Shader> mh_vs;
        std::weak_ptr<Shader> mh_ps;

        std::vector<vk::DescriptorSetLayout>   m_descriptorset_layouts;
        std::vector<Descriptorset_layoutdata*> m_descriptorset_infos;

        vk::PipelineLayout m_pipeline_layout;
        vk::Pipeline       m_pipeline;

        inline static const std::string ENTRY_POINT = "main";
    };

} // namespace VKN
