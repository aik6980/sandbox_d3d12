#pragma once

namespace VKN {
    class Technique;
    struct Buffer;

    class Technique_instance {
      public:
        Technique_instance(Technique& tech)
            : m_tech(tech)
        {
        }

        void set_constant(const std::string name, const void* data, size_t size);

        void set_descriptor_set_parameters();

        Technique& m_tech;

        std::unordered_map<std::string, std::weak_ptr<Buffer>> m_constant_buffer_map;
    };
} // namespace VKN
