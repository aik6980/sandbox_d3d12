#pragma once
#include "engine_graphic/engine_graphic.h"
#include "shaders/hlsl_shared_struct.h"
#include "world/component/camera.h"

class Mesh_renderer;
class Raytrace_renderer;
class Post_renderer;

class Render_pass_main;
class Render_pass_shadow_map;
class Render_pass_raytrace_main;

class Frame_pipeline : public Engine_client {
  public:
    Frame_pipeline(Engine& engine);

    virtual void load_resource() override;
    virtual void update() override;
    virtual void draw() override;

    unique_ptr<Mesh_renderer>     m_mesh_renderer;
    unique_ptr<Raytrace_renderer> m_raytrace_renderer;
    unique_ptr<Post_renderer>     m_post_renderer;

    unique_ptr<Render_pass_main>          m_render_pass_main;
    unique_ptr<Render_pass_shadow_map>    m_render_pass_shadow_map;
    unique_ptr<Render_pass_raytrace_main> m_render_pass_raytrace_main;

    Camera m_camera;

  private:
    Engine& m_engine;
};
