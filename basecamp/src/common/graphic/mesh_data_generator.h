#pragma once

#include "graphic.h"
#include "mesh_data.h"

class MeshDataGenerator {
  public:
    static Mesh_data create_unit_quad();
    static Mesh_data create_unit_cube();
    static Mesh_data create_grid(float width, float depth, uint32_t m, uint32_t n);

    static vector<Fat_vertex> to_fat(const Mesh_vertex_array& vertex_array);
    static vector<RT_vertex>  to_rt(const Mesh_vertex_array& vertex_array);
    static vector<P1_vertex>  to_p1(const Mesh_vertex_array& vertex_array);
    static vector<PC_vertex>  to_p1c1(const Mesh_vertex_array& vertex_array);
};

class TextureDataGenerator {
  public:
    static TextureData create_checkerboard_texture(int texture_width);
};
