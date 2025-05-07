#pragma once

namespace VKN {

    class Device;
    class Shader;

    struct Descriptorset_layoutdata;

    struct Technique_createinfo {
        std::string m_vs_name;
        std::string m_ps_name;
    };

    struct Targets_createinfo {
        vk::Format m_colour_format = vk::Format::eUndefined;
        vk::Format m_depth_format  = vk::Format::eUndefined;
    };

    class Technique {

      public:
        Technique(Device& gfx_device)
            : m_gfx_device(gfx_device)
        {
        }

        void destroy();

        // void create_pipeline();
        void create_pipeline(vk::Format color_format, vk::Format depth_format);

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
