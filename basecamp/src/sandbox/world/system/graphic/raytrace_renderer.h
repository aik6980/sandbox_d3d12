#pragma once

#include "engine_graphic/engine_graphic.h"
#include "render_pass/render_pass_main.h"
#include "world/component/camera.h"

class Engine;

class Raytrace_renderer : public Engine_client {
  public:
    Raytrace_renderer(Engine& engine) : m_engine(engine) {}

    void         init();
    virtual void load_resource() override;
    virtual void update() override{};
    virtual void draw() override;

  private:
    unique_ptr<Render_pass_main> m_render_pass_main;

    shared_ptr<D3D12::Lib_ray_technique_instance> m_raytrace_technique_instance;

    Engine& m_engine;
};
