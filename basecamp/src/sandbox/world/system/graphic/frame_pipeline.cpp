#include "frame_pipeline.h"

#include "common/common_cpp.h"
#include "engine_graphic/engine_graphic_cpp.h"
#include "mesh_renderer.h"
#include "raytrace_renderer.h"
#include "render_pass/render_pass_main.h"

Frame_pipeline::Frame_pipeline(Engine& engine) : m_engine(engine) {}

void Frame_pipeline::load_resource()
{
    m_mesh_renderer = std::make_unique<Mesh_renderer>(m_engine, *this);
    m_mesh_renderer->load_resource();
    m_raytrace_renderer = std::make_unique<Raytrace_renderer>(m_engine, *this);
    m_raytrace_renderer->load_resource();
    m_post_renderer = std::make_unique<Post_renderer>(m_engine, *this);
    m_post_renderer->load_resource();

    m_render_pass_main = make_unique<Render_pass_main>();
    m_render_pass_main->load_resource();
    m_render_pass_shadow_map = make_unique<Render_pass_shadow_map>();
    m_render_pass_shadow_map->load_resource();

    // setup camera
    auto&& render_device = m_engine.render_device();
    auto&& viewport      = render_device.get_window_viewport();
    auto&& aspect_ratio  = viewport.Width / viewport.Height;

    XMMATRIX projection   = Matrix::CreatePerspectiveFieldOfViewLH(0.4f * XM_PI, aspect_ratio, 1.0f, 1000.0f);
    m_camera.m_projection = projection;
}

void Frame_pipeline::update()
{
    m_mesh_renderer->update();
    m_raytrace_renderer->update();
}

void Frame_pipeline::draw()
{
    m_mesh_renderer->draw();
    m_raytrace_renderer->draw();
    m_post_renderer->draw();
}
