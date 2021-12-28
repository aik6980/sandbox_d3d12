#pragma once

struct Shader_record {

    Shader_record(void* shader_identifier, uint32_t shader_identifier_size) : ptr(shader_identifier), size(shader_identifier_size) {}

    void*    ptr  = nullptr;
    uint32_t size = 0;
};

struct Shader_table {
    vector<Shader_record> m_shader_records;

    vector<uint8_t> generate_data();
};
