#pragma once

#include "graphic.h"
#include "mesh_data.h"

class MeshDataGenerator {
  public:
    static void                                   create_unit_quad(MeshVertexArray& vertex_array, MeshIndexArray& index_array);
    static tuple<MeshVertexArray, MeshIndexArray> create_unit_cube();
    static tuple<MeshVertexArray, MeshIndexArray> create_grid(float width, float depth, uint32_t m, uint32_t n);

    static vector<RtInputLayout>   to_rt(MeshVertexArray& vertex_array);
    static vector<P1InputLayout>   to_p1(MeshVertexArray& vertex_array);
    static vector<P1C1InputLayout> to_p1c1(MeshVertexArray& vertex_array);
};

class TextureDataGenerator {
  public:
    static TextureData create_checkerboard_texture(int texture_width);
};
