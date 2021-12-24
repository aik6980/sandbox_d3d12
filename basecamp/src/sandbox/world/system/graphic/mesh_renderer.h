#pragma once

#include "engine_graphic/engine_graphic.h"
#include "render_pass/render_pass_main.h"
#include "world/component/camera.h"

class Engine;

class MeshRenderer : public EngineClient {
  public:
    MeshRenderer(Engine& engine);

    void         init();
    virtual void load_resource() override;
    virtual void update() override{};
    virtual void draw() override;

  private:
    struct Instance_data {
        XMFLOAT4 pos_world;
        XMFLOAT4 orient_world;
    };
    int                   m_num_instances = 5;
    vector<Instance_data> m_instance_data;

    void draw_meshes_shadow_map();
    void draw_meshes();
    void process_post();
    void copy_to_backbuffer();

    void build_quad_mesh();
    void build_cube_mesh();

    void build_texture();

    void update_camera();

    Engine& m_engine;

    string m_unit_quad_name;
    string m_unit_cube_name;
    string m_grid_mesh;

    string m_texture_name              = "checkerboard_texture";
    string m_point_sampler             = "point_sampler";
    string m_instance_data_buffer_name = "instance_data_buffer";

    Camera m_camera;
    Camera m_light;

    shared_ptr<D3D12::TechniqueInstance> m_render_technique_instance;
    shared_ptr<D3D12::TechniqueInstance> m_render_technique_instance1;

    shared_ptr<D3D12::TechniqueInstance> m_render_technique_mesh_instance;
    shared_ptr<D3D12::TechniqueInstance> m_render_technique_grid_mesh_instance;

    shared_ptr<D3D12::TechniqueInstance> m_shadow_map_technique_instance;

    shared_ptr<D3D12::TechniqueInstance> m_mesh_instancing_technique_instance;

    shared_ptr<D3D12::TechniqueInstance> m_compute_post_technique_instance;

    unique_ptr<Render_pass_main>       m_render_pass_main;
    unique_ptr<Render_pass_shadow_map> m_render_pass_shadow_map;
};
