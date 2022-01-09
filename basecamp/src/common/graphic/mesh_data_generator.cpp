#include "mesh_data_generator.h"
#include "graphic_cpp.h"

void MeshDataGenerator::create_unit_quad(MeshVertexArray& vertex_array, MeshIndexArray& index_array)
{
    vertex_array.reset_vertices(4);
    index_array.reset_indices(6);

    vertex_array.m_position[0] = XMFLOAT3(-0.5f, 0.5f, 0.0f);
    vertex_array.m_position[1] = XMFLOAT3(0.5f, 0.5f, 0.0f);
    vertex_array.m_position[2] = XMFLOAT3(-0.5f, -0.5f, 0.0f);
    vertex_array.m_position[3] = XMFLOAT3(0.5f, -0.5f, 0.0f);

    index_array.m_indices32[0] = 0;
    index_array.m_indices32[1] = 1;
    index_array.m_indices32[2] = 2;

    index_array.m_indices32[3] = 2;
    index_array.m_indices32[4] = 1;
    index_array.m_indices32[5] = 3;
}

tuple<MeshVertexArray, MeshIndexArray> MeshDataGenerator::create_unit_cube()
{
    MeshVertexArray vertex_array;
    MeshIndexArray  index_array;
    vertex_array.reset_vertices(4 * 6);
    index_array.reset_indices(6 * 6);

    // +/- x
    int offset                 = 0;
    vertex_array.m_position[0] = XMFLOAT3(-0.5f, -0.5f, 0.5f);
    vertex_array.m_position[1] = XMFLOAT3(-0.5f, 0.5f, 0.5f);
    vertex_array.m_position[2] = XMFLOAT3(-0.5f, -0.5f, -0.5f);
    vertex_array.m_position[3] = XMFLOAT3(-0.5f, 0.5f, -0.5f);

    vertex_array.m_position[4] = XMFLOAT3(0.5f, -0.5f, 0.5f);
    vertex_array.m_position[5] = XMFLOAT3(0.5f, 0.5f, 0.5f);
    vertex_array.m_position[6] = XMFLOAT3(0.5f, -0.5f, -0.5f);
    vertex_array.m_position[7] = XMFLOAT3(0.5f, 0.5f, -0.5f);

    for (int i = offset; i < offset + 4; ++i) {
        vertex_array.m_colour[i]     = XMFLOAT3(1.0f, 0.1f, 0.1f);
        vertex_array.m_colour[i + 4] = XMFLOAT3(0.5f, 0.1f, 0.1f);
    }

    // +/- y
    offset                              = 8;
    vertex_array.m_position[offset + 0] = XMFLOAT3(-0.5f, 0.5f, 0.5f);
    vertex_array.m_position[offset + 1] = XMFLOAT3(0.5f, 0.5f, 0.5f);
    vertex_array.m_position[offset + 2] = XMFLOAT3(-0.5f, 0.5f, -0.5f);
    vertex_array.m_position[offset + 3] = XMFLOAT3(0.5f, 0.5f, -0.5f);

    vertex_array.m_position[offset + 4] = XMFLOAT3(-0.5f, -0.5f, 0.5f);
    vertex_array.m_position[offset + 5] = XMFLOAT3(0.5f, -0.5f, 0.5f);
    vertex_array.m_position[offset + 6] = XMFLOAT3(-0.5f, -0.5f, -0.5f);
    vertex_array.m_position[offset + 7] = XMFLOAT3(0.5f, -0.5f, -0.5f);

    for (int i = offset; i < offset + 4; ++i) {
        vertex_array.m_colour[i]     = XMFLOAT3(0.1f, 1.0f, 0.1f);
        vertex_array.m_colour[i + 4] = XMFLOAT3(0.1f, 0.5f, 0.1f);
    }

    // +/-z
    offset                              = 16;
    vertex_array.m_position[offset + 0] = XMFLOAT3(-0.5f, 0.5f, -0.5f);
    vertex_array.m_position[offset + 1] = XMFLOAT3(0.5f, 0.5f, -0.5f);
    vertex_array.m_position[offset + 2] = XMFLOAT3(-0.5f, -0.5f, -0.5f);
    vertex_array.m_position[offset + 3] = XMFLOAT3(0.5f, -0.5f, -0.5f);

    vertex_array.m_position[offset + 4] = XMFLOAT3(-0.5f, 0.5f, 0.5f);
    vertex_array.m_position[offset + 5] = XMFLOAT3(0.5f, 0.5f, 0.5f);
    vertex_array.m_position[offset + 6] = XMFLOAT3(-0.5f, -0.5f, 0.5f);
    vertex_array.m_position[offset + 7] = XMFLOAT3(0.5f, -0.5f, 0.5f);

    for (int i = offset; i < offset + 4; ++i) {
        vertex_array.m_colour[i]     = XMFLOAT3(0.1f, 0.1f, 1.0f);
        vertex_array.m_colour[i + 4] = XMFLOAT3(0.1f, 0.1f, 0.5f);
    }

    // indices
    int ioffset                           = 0;
    int voffset1                          = 0;
    int voffset2                          = voffset1 + 4;
    index_array.m_indices32[ioffset + 0]  = voffset1 + 0;
    index_array.m_indices32[ioffset + 1]  = voffset1 + 1;
    index_array.m_indices32[ioffset + 2]  = voffset1 + 2;
    index_array.m_indices32[ioffset + 3]  = voffset1 + 2;
    index_array.m_indices32[ioffset + 4]  = voffset1 + 1;
    index_array.m_indices32[ioffset + 5]  = voffset1 + 3;
    index_array.m_indices32[ioffset + 6]  = voffset2 + 2;
    index_array.m_indices32[ioffset + 7]  = voffset2 + 1;
    index_array.m_indices32[ioffset + 8]  = voffset2 + 0;
    index_array.m_indices32[ioffset + 9]  = voffset2 + 3;
    index_array.m_indices32[ioffset + 10] = voffset2 + 1;
    index_array.m_indices32[ioffset + 11] = voffset2 + 2;

    ioffset                               = 12;
    voffset1                              = 8;
    voffset2                              = voffset1 + 4;
    index_array.m_indices32[ioffset + 0]  = voffset1 + 0;
    index_array.m_indices32[ioffset + 1]  = voffset1 + 1;
    index_array.m_indices32[ioffset + 2]  = voffset1 + 2;
    index_array.m_indices32[ioffset + 3]  = voffset1 + 2;
    index_array.m_indices32[ioffset + 4]  = voffset1 + 1;
    index_array.m_indices32[ioffset + 5]  = voffset1 + 3;
    index_array.m_indices32[ioffset + 6]  = voffset2 + 2;
    index_array.m_indices32[ioffset + 7]  = voffset2 + 1;
    index_array.m_indices32[ioffset + 8]  = voffset2 + 0;
    index_array.m_indices32[ioffset + 9]  = voffset2 + 3;
    index_array.m_indices32[ioffset + 10] = voffset2 + 1;
    index_array.m_indices32[ioffset + 11] = voffset2 + 2;

    ioffset                               = 24;
    voffset1                              = 16;
    voffset2                              = voffset1 + 4;
    index_array.m_indices32[ioffset + 0]  = voffset1 + 0;
    index_array.m_indices32[ioffset + 1]  = voffset1 + 1;
    index_array.m_indices32[ioffset + 2]  = voffset1 + 2;
    index_array.m_indices32[ioffset + 3]  = voffset1 + 2;
    index_array.m_indices32[ioffset + 4]  = voffset1 + 1;
    index_array.m_indices32[ioffset + 5]  = voffset1 + 3;
    index_array.m_indices32[ioffset + 6]  = voffset2 + 2;
    index_array.m_indices32[ioffset + 7]  = voffset2 + 1;
    index_array.m_indices32[ioffset + 8]  = voffset2 + 0;
    index_array.m_indices32[ioffset + 9]  = voffset2 + 3;
    index_array.m_indices32[ioffset + 10] = voffset2 + 1;
    index_array.m_indices32[ioffset + 11] = voffset2 + 2;

    return tuple(vertex_array, index_array);
}

tuple<MeshVertexArray, MeshIndexArray> MeshDataGenerator::create_grid(float width, float depth, uint32_t m, uint32_t n)
{
    uint32_t vertex_count = m * n;
    uint32_t face_count   = (m - 1) * (n - 1) * 2;

    //
    // Create the vertices.
    //

    float half_width = 0.5f * width;
    float half_depth = 0.5f * depth;

    float dx = width / (n - 1);
    float dz = depth / (m - 1);

    float du = 1.0f / (n - 1);
    float dv = 1.0f / (m - 1);

    MeshVertexArray vertices;
    vertices.reset_vertices(vertex_count);

    for (uint32_t i = 0; i < m; ++i) {
        float z = half_depth - i * dz;
        for (uint32_t j = 0; j < n; ++j) {
            float x = -half_width + j * dx;

            uint32_t idx = i * n + j;

            vertices.m_position[idx] = XMFLOAT3(x, 0.0f, z);
            vertices.m_colour[idx]   = XMFLOAT3(0.6f, 0.6f, 0.6f);
            // vertices[i * n + j].Normal   = XMFLOAT3(0.0f, 1.0f, 0.0f);
            // vertices[i * n + j].TangentU = XMFLOAT3(1.0f, 0.0f, 0.0f);

            // Stretch texture over grid.
            // meshData.Vertices[i * n + j].TexC.x = j * du;
            // meshData.Vertices[i * n + j].TexC.y = i * dv;
        }
    }

    //
    // Create the indices.
    //

    MeshIndexArray indices;
    indices.reset_indices(face_count * 3); // 3 indices per face

    // Iterate over each quad and compute indices.
    uint32_t k = 0;
    for (uint32_t i = 0; i < m - 1; ++i) {
        for (uint32_t j = 0; j < n - 1; ++j) {
            indices.m_indices32[k]     = i * n + j;
            indices.m_indices32[k + 1] = i * n + j + 1;
            indices.m_indices32[k + 2] = (i + 1) * n + j;

            indices.m_indices32[k + 3] = (i + 1) * n + j;
            indices.m_indices32[k + 4] = i * n + j + 1;
            indices.m_indices32[k + 5] = (i + 1) * n + j + 1;

            k += 6; // next quad
        }
    }

    return tuple(vertices, indices);
}

vector<Fat_vertex> MeshDataGenerator::to_fat(MeshVertexArray& vertex_array)
{
    vector<Fat_vertex> output;
    output.resize(vertex_array.m_position.size());
    for (uint32_t i = 0; i < vertex_array.m_position.size(); ++i) {
        XMStoreFloat3(&output[i].m_position, XMLoadFloat3(&vertex_array.m_position[i]));
        XMStoreFloat4(&output[i].m_colour, XMLoadFloat3(&vertex_array.m_colour[i]));
        XMStoreFloat3(&output[i].m_normal, XMLoadFloat3(&vertex_array.m_normal[i]));
    }

    return output;
}

vector<RT_vertex> MeshDataGenerator::to_rt(MeshVertexArray& vertex_array)
{
    vector<RT_vertex> output;
    output.resize(vertex_array.m_position.size());
    for (uint32_t i = 0; i < vertex_array.m_position.size(); ++i) {
        XMStoreFloat3(&output[i].m_position, XMLoadFloat3(&vertex_array.m_position[i]));
    }

    return output;
}

vector<P1_vertex> MeshDataGenerator::to_p1(MeshVertexArray& vertex_array)
{
    vector<P1_vertex> output;
    output.resize(vertex_array.m_position.size());
    for (uint32_t i = 0; i < vertex_array.m_position.size(); ++i) {
        XMStoreFloat4(&output[i].m_position, XMLoadFloat3(&vertex_array.m_position[i]));
    }

    return output;
}

vector<P1C1_vertex> MeshDataGenerator::to_p1c1(MeshVertexArray& vertex_array)
{
    vector<P1C1_vertex> output;
    output.resize(vertex_array.m_position.size());
    for (uint32_t i = 0; i < vertex_array.m_position.size(); ++i) {
        XMStoreFloat4(&output[i].m_position, XMLoadFloat3(&vertex_array.m_position[i]));
        XMStoreFloat4(&output[i].m_colour, XMLoadFloat3(&vertex_array.m_colour[i]));
    }

    return output;
}

//////////////////////////////////////////////////////////////////////////

TextureData TextureDataGenerator::create_checkerboard_texture(int texture_width)
{
    TextureData texture_data;
    texture_data.reset(texture_width, texture_width);
    float value;
    for (int row = 0; row < texture_width; row++) {
        for (int col = 0; col < texture_width; col++) {
            // Each cell is 8x8, value is 0 or 1 (black or white)
            value = (((row & 0x8) == 0) ^ ((col & 0x8) == 0)) * 1.0f;
            XMCOLOR colour(value, value, value, 1.0f);
            texture_data.set_data(row, col, colour);
        }
    }

    return texture_data;
}
