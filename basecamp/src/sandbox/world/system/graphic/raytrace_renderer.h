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

    Engine& m_engine;
};
