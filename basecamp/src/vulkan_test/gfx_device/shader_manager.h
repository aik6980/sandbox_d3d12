#pragma once

#include "shader.h"
#include "technique.h"

namespace VKN {
    class Shader_manager {
      public:
        Shader_manager(Device& gfx_device)
            : m_gfx_device(gfx_device)
        {
        }

        void destroy_resources();

        std::weak_ptr<Technique> register_technique(std::string name, const Technique_createinfo& create_info);
        std::weak_ptr<Technique> get_technique(std::string name);

      private:
        std::weak_ptr<Shader> register_shader(std::string name);
        std::weak_ptr<Shader> get_shader(std::string name);

        Device& m_gfx_device;

        std::unordered_map<std::string, std::shared_ptr<Shader>>    m_shader_map;
        std::unordered_map<std::string, std::shared_ptr<Technique>> m_technique_map;
    };
} // namespace VKN
