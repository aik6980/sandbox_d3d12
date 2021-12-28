#include "lib_ray_shader_table.h"

vector<uint8_t> Shader_table::generate_data()
{
    vector<uint8_t> data;

    // calculate aligned buffer size, offset
    auto             total_size = 0;
    vector<uint32_t> offsets(m_shader_records.size());
    vector<uint32_t> aligned_sizes(m_shader_records.size());
    for (int i = 0; i < m_shader_records.size(); ++i) {
        aligned_sizes[i] = Align_up(m_shader_records[i].size, (uint32_t)D3D12_RAYTRACING_SHADER_RECORD_BYTE_ALIGNMENT);
        offsets[i]       = total_size;

        total_size += aligned_sizes[i];
    }

    data.resize(total_size);

    // copy shader records into data blob
    for (int i = 0; i < m_shader_records.size(); ++i) {
        void* dest = data.data() + offsets[i];

        memcpy(dest, m_shader_records[i].ptr, aligned_sizes[i]);
    }

    return std::move(data);
}
