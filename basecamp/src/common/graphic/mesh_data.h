#pragma once

#include "math/aabb.h"

struct Mesh_vertex_array {
    // vertices
    vector<Vector3> m_position;
    vector<Vector3> m_normal;
    vector<Color>   m_colour;

    void reset_vertices(UINT n);
};

struct Mesh_index_array {
    // indices
    vector<uint32_t> m_indices32;

    void reset_indices(UINT n);
};

struct Mesh_data {
    Mesh_vertex_array m_vertices;
    Mesh_index_array  m_indices;

    Aabb3 m_aabb;
};

struct TextureData {
    uint32_t        m_width  = 0;
    uint32_t        m_height = 0;
    vector<XMCOLOR> m_data;

    void reset(uint32_t w, uint32_t h);
    void set_data(uint32_t x, uint32_t y, const XMCOLOR& val);
    int  pixel_size_in_byte() const
    {
        return sizeof(decltype(*m_data.begin()));
    }
};

struct RT_vertex {
    XMFLOAT3 m_position;
};

struct P1_vertex {
    XMFLOAT4 m_position;
};
