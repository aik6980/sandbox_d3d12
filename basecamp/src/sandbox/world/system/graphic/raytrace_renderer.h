#pragma once

#include "engine_graphic/engine_graphic.h"
#include "render_pass/render_pass_main.h"
#include "world/component/camera.h"

class Engine;
class Frame_pipeline;

struct Mesh_data_raw {
    unique_ptr<MeshVertexArray> vertices_raw;
    unique_ptr<MeshIndexArray>  indices_raw;

    vector<Fat_vertex> vertices_fat;
};

struct Scene_data_buffer {
    weak_ptr<D3D12::Buffer> instance_data_buffer;
    weak_ptr<D3D12::Buffer> mesh_data_buffer;
    weak_ptr<D3D12::Buffer> vertex_buffer;
    weak_ptr<D3D12::Buffer> index_buffer;
};

class Raytrace_renderer {
  public:
    Raytrace_renderer(Engine& engine, Frame_pipeline& frame_pipeline) : m_engine(engine), m_frame_pipeline(frame_pipeline) {}

    void init();
    void load_resource();
    void update();
    void draw();

    Scene_data_buffer build_scene_attrib_buffer();

  private:
    // shared_ptr<D3D12::Lib_ray_technique_instance> m_raytrace_technique_instance;
    string m_technique_raytracing_simple_lighting;

    string m_rtaccel_structure_buffer_handle;
    // string m_unit_quad_name;
    string m_unit_cube_name;
    string m_grid_mesh;

    unique_ptr<D3D12::Scene_data> m_scene_data;

    Engine&         m_engine;
    Frame_pipeline& m_frame_pipeline;

    unordered_map<string, unique_ptr<Mesh_data_raw>> m_mesh_list;
};
