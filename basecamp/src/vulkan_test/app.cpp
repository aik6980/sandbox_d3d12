#include "app.h"

#include <atomic>
#include <thread>

#include "common/common_cpp.h"
#include "global.h"

#include "gfx_device/gfx_main.h"
#include "main_renderer.h"

std::chrono::time_point<std::chrono::steady_clock> App::m_time_begin_app;
std::chrono::time_point<std::chrono::steady_clock> App::m_time_begin_frame;
std::chrono::microseconds App::m_duration_frame;

std::unique_ptr<std::thread> render_thread;
std::atomic<bool> game_running = true;

Main_renderer main_renderer;

void render_thread_func()
{
    while (game_running) {

        Sleep(1000);
        OutputDebugString(L"render frame\n");
    }
}

void App::on_init(HINSTANCE hInstance, HWND hWnd)
{
    m_hInstance = hInstance;
    m_hWnd      = hWnd;

    OutputDebugString(L"app start\n");

    // time
    m_time_begin_frame = m_time_begin_app = std::chrono::high_resolution_clock::now();

    // m_engine->update();
    // render thread
    render_thread.reset(new std::thread(render_thread_func));

    Gfx_main::create(m_hInstance, m_hWnd);

    auto&& gfx_device = Gfx_main::gfx_device();

    // load resources
    gfx_device.begin_single_command_submission();
    create_scene();
    gfx_device.end_single_command_submission();
}

void App::on_update()
{
    // frame time
    m_duration_frame =
        std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - m_time_begin_frame);
    m_time_begin_frame = std::chrono::steady_clock::now();

    auto&& debug_str = DBG::Format(L"Cureent frame time %.4f ms", m_duration_frame.count() / 1000.0f);
    SetWindowText(m_hWnd, debug_str.c_str());

    // render the scene
    auto&& gfx_device = Gfx_main::gfx_device();

    gfx_device.begin_frame();
    main_renderer.draw();
    gfx_device.end_frame();
}

void App::on_destroy()
{
    game_running = false;

    render_thread->join();

    Gfx_main::destroy();

    OutputDebugString(L"app destroy\n");
}

void App::create_scene()
{
    auto&& gfx_device = Gfx_main::gfx_device();

    auto&& shader_manager = Gfx_main::shader_manager();
    // auto&& resource_manager = gfx_device.m_resource_manager;

    // for each render passes
    auto&& colour_format                 = gfx_device.backbuffer_colour_format();
    auto&& depth_format                  = gfx_device.backbuffer_depth_format();
    VKN::Targets_createinfo targets_info = {colour_format, depth_format};
    // create techniques
    shader_manager.register_technique("test/single_triangle", targets_info);
    shader_manager.register_technique("test/bindless_textures", targets_info);

    shader_manager.register_technique("t1",
        VKN::Technique_createinfo{.m_vs_name = "hello_triangle_mesh.vs", .m_ps_name = "hello_triangle.ps"},
        targets_info);

    // create mesh
    // resource_manager->create_mesh();

    // shader_manager->register_shader("hello_triangle.vs");
    // shader_manager->register_shader("hello_triangle_mesh.vs");
    // shader_manager->register_shader("hello_triangle.ps");
}
