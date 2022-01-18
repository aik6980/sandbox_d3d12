#pragma once

#include "graphic.h"
#include "mesh_data.h"

class MeshDataGenerator {
  public:
    static void                                       create_unit_quad(Mesh_vertex_array& vertex_array, Mesh_index_array& index_array);
    static tuple<Mesh_vertex_array, Mesh_index_array> create_unit_cube();
    static tuple<Mesh_vertex_array, Mesh_index_array> create_grid(float width, float depth, uint32_t m, uint32_t n);

    static vector<Fat_vertex>  to_fat(const Mesh_vertex_array& vertex_array);
    static vector<RT_vertex>   to_rt(const Mesh_vertex_array& vertex_array);
    static vector<P1_vertex>   to_p1(const Mesh_vertex_array& vertex_array);
    static vector<P1C1_vertex> to_p1c1(const Mesh_vertex_array& vertex_array);
};

class TextureDataGenerator {
  public:
    static TextureData create_checkerboard_texture(int texture_width);
};
