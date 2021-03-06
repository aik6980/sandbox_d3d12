#include "app.h"

#include <atomic>
#include <thread>

#include "common/common_cpp.h"
#include "engine_graphic/engine_graphic_cpp.h"
#include "input/input_manager.h"
#include "world/component/components.h"
#include "world/entity/entity.h"
#include "world/system/graphic/frame_pipeline.h"
#include "world/system/graphic/mesh_renderer.h"
#include "world/system/graphic/raytrace_renderer.h"
#include "world/system/graphic/render_pass/render_pass_main.h"
#include "world/system/systems.h"

std::unique_ptr<Engine>        App::m_engine;
std::unique_ptr<Input_manager> App::m_input_manager;
entt::registry                 App::m_reg;

Frame_pipeline* App::m_renderer = nullptr;

std::chrono::time_point<std::chrono::steady_clock> App::m_time_begin_app;
std::chrono::time_point<std::chrono::steady_clock> App::m_time_begin_frame;
std::chrono::microseconds                          App::m_duration_frame;

std::unique_ptr<std::thread> render_thread;
std::atomic<bool>            game_running = true;

void App::on_init(HINSTANCE hInstance, HWND hWnd)
{
    m_hInstance = hInstance;
    m_hWnd      = hWnd;

    m_input_manager = make_unique<Input_manager>();
    m_input_manager->init(m_hWnd);

    m_engine = make_unique<Engine>();
    Engine::InitData init_data;
    init_data.HWnd = hWnd;
    m_engine->init(init_data);

    unique_ptr<Frame_pipeline> renderer(new Frame_pipeline(*m_engine));
    m_renderer       = renderer.get();
    renderer->m_name = "renderer";
    m_engine->register_client(std::move(renderer));

    m_engine->load_resource();

    // time
    m_time_begin_frame = m_time_begin_app = std::chrono::high_resolution_clock::now();

    // world
    create_world();

    // thread
    // m_engine->update();
    // render_thread.reset(new std::thread(
    //	[&]() { while (game_running) { m_engine->draw(); }
    //}));
}

void App::on_update()
{
    // frame time
    m_duration_frame   = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - m_time_begin_frame);
    m_time_begin_frame = std::chrono::steady_clock::now();

    // input
    m_input_manager->update();

    // system
    Player_input(m_reg);
    Camera_update(m_reg);

    m_engine->update();
    m_engine->draw();
}

void App::on_destroy()
{
    game_running = false;
    // render_thread->join();

    m_engine->destroy();
    m_input_manager->destroy();
}

void App::create_world()
{
    // create a player
    Make_player(m_reg);
    Make_camera(m_reg);
}
