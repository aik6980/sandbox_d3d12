#pragma once

#include "engine_graphic/engine_graphic.h"
#include "render_pass/render_pass_main.h"
#include "world/component/camera.h"

class Engine;
class Frame_pipeline;

class Raytrace_renderer {
  public:
    Raytrace_renderer(Engine& engine, Frame_pipeline& frame_pipeline) : m_engine(engine), m_frame_pipeline(frame_pipeline) {}

    void init();
    void load_resource();
    void update();
    void draw();

  private:
    shared_ptr<D3D12::Lib_ray_technique_instance> m_raytrace_technique_instance;

    string m_rtaccel_structure_buffer_handle;
    // string m_unit_quad_name;
    string m_grid_mesh;

    Engine&         m_engine;
    Frame_pipeline& m_frame_pipeline;
};
