#include "app.h"

#include <atomic>
#include <thread>

#include "common/common_cpp.h"
#include "gfx_device/device.h"
#include "gfx_device/resource_manager.h"
#include "gfx_device/shader_manager.h"
#include "gfx_device/technique.h"

std::chrono::time_point<std::chrono::steady_clock> App::m_time_begin_app;
std::chrono::time_point<std::chrono::steady_clock> App::m_time_begin_frame;
std::chrono::microseconds                          App::m_duration_frame;

std::unique_ptr<std::thread> render_thread;
std::atomic<bool>            game_running = true;

VKN::Device m_gfx_device;

void App::on_init(HINSTANCE hInstance, HWND hWnd)
{
    m_hInstance = hInstance;
    m_hWnd      = hWnd;

    // time
    m_time_begin_frame = m_time_begin_app = std::chrono::high_resolution_clock::now();

    // thread
    // m_engine->update();
    // render_thread.reset(new std::thread(
    //	[&]() { while (game_running) { m_engine->draw(); }
    //}));

    m_gfx_device.create(m_hInstance, m_hWnd);
    // device.draw();
    m_gfx_device.load_resources();

    m_gfx_device.begin_single_command_submission();
    create_scene();
    m_gfx_device.end_single_command_submission();
}

void App::on_update()
{
    // frame time
    m_duration_frame   = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - m_time_begin_frame);
    m_time_begin_frame = std::chrono::steady_clock::now();

    m_gfx_device.draw();
}

void App::on_destroy()
{
    game_running = false;
    // render_thread->join();

    m_gfx_device.destroy();
}

void App::create_scene()
{
    auto&& shader_manager   = m_gfx_device.m_shader_manager;
    auto&& resource_manager = m_gfx_device.m_resource_manager;

    // for each render passes
    auto&& colour_format = m_gfx_device.get_backbuffer_colour_format();
    auto&& depth_format = m_gfx_device.get_backbuffer_depth_format();
    VKN::Targets_createinfo targets_info = {colour_format, depth_format};
    // create techniques
    shader_manager->register_technique("t0", VKN::Technique_createinfo{.m_vs_name = "hello_triangle.vs", .m_ps_name = "hello_triangle.ps"}, targets_info);
    shader_manager->register_technique("t1", VKN::Technique_createinfo{.m_vs_name = "hello_triangle_mesh.vs", .m_ps_name = "hello_triangle.ps"}, targets_info);

    // create mesh
    resource_manager->create_mesh();

    // shader_manager->register_shader("hello_triangle.vs");
    // shader_manager->register_shader("hello_triangle_mesh.vs");
    // shader_manager->register_shader("hello_triangle.ps");
}
