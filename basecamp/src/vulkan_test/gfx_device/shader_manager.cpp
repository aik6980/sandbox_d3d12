#include "shader_manager.h"

namespace VKN {

    void Shader_manager::destroy_resources()
    {
        for (auto&& itr : m_technique_map) {
            itr.second->destroy();
        }
        m_technique_map.clear();

        for (auto&& itr : m_shader_map) {
            itr.second->destroy();
        }
        m_shader_map.clear();
    }

    std::weak_ptr<Technique> Shader_manager::register_technique(
        const std::string& filename, const Targets_createinfo& targets)
    {
        auto create_info = Technique_createinfo{
            .m_vs_name = filename + ".vs",
            .m_ps_name = filename + ".ps",
        };

        auto&& technique = register_technique(filename, create_info, targets);

        return technique;
    }

    std::weak_ptr<Technique> Shader_manager::register_technique(
        const std::string& name, const Technique_createinfo& create_info, const Targets_createinfo& targets)
    {
        if (auto&& t = get_technique(name); t.lock() != nullptr) {
            return t;
        }

        auto&& h_vs_shader = register_shader(create_info.m_vs_name);
        auto&& h_ps_shader = register_shader(create_info.m_ps_name);

        auto&& technique = std::make_shared<Technique>(m_gfx_device);
        technique->mh_vs = h_vs_shader;
        technique->mh_ps = h_ps_shader;
        technique->create_pipeline(targets.m_colour_format, targets.m_depth_format);

        m_technique_map.insert({name, technique});
        return technique;
    }

    std::weak_ptr<Technique> Shader_manager::get_technique(std::string name)
    {
        auto&& itr = m_technique_map.find(name);
        if (itr != std::end(m_technique_map)) {
            return itr->second;
        }

        return std::weak_ptr<Technique>();
    }

    std::weak_ptr<Shader> Shader_manager::register_shader(std::string name)
    {
        if (auto&& sh = get_shader(name); sh.lock() != nullptr) {
            return sh;
        }

        auto&& shader = std::make_shared<Shader>(m_gfx_device);
        shader->create_shader(name);

        m_shader_map.insert({name, shader});
        return shader;
    }

    std::weak_ptr<Shader> Shader_manager::get_shader(std::string name)
    {
        auto&& result = m_shader_map.find(name);
        if (result != m_shader_map.end()) {
            return result->second;
        }

        return std::weak_ptr<Shader>();
    }

} // namespace VKN
