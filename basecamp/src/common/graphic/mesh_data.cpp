#include "mesh_data.h"

void Mesh_vertex_array::reset_vertices(UINT n)
{
    m_position.resize(n);
    m_normal.resize(n);
    m_colour.resize(n);
}

void Mesh_index_array::reset_indices(UINT n)
{
    m_indices32.resize(n);
}

void TextureData::reset(uint32_t w, uint32_t h)
{
    m_width  = w;
    m_height = h;
    m_data.resize(w * h);
}

void TextureData::set_data(uint32_t x, uint32_t y, const XMCOLOR& val)
{
    uint32_t id = y * m_width + x;
    m_data[id]  = val;
}
