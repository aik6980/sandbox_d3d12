#pragma once

namespace VKN {
    class Technique;
    class Buffer;

    class Technique_instance {
      public:
        Technique_instance(Technique& tech)
            : m_tech(tech)
        {
        }

        void set_constant(const std::string name, const void* data, size_t size);

        Technique& m_tech;

        std::unordered_map<std::string, std::weak_ptr<Buffer>> m_constant_buffer_map;
    };
} // namespace VKN
