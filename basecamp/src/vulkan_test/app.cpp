#include "app.h"

#include <atomic>
#include <thread>

#include "common/common_cpp.h"
#include "gfx_device/device.h"
#include "gfx_device/resource_manager.h"
#include "gfx_device/shader_manager.h"

std::chrono::time_point<std::chrono::steady_clock> App::m_time_begin_app;
std::chrono::time_point<std::chrono::steady_clock> App::m_time_begin_frame;
std::chrono::microseconds                          App::m_duration_frame;

std::unique_ptr<std::thread> render_thread;
std::atomic<bool>            game_running = true;

VKN::Device device;

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

    device.create(m_hInstance, m_hWnd);
    device.load_resources();

    // device.draw();
}

void App::on_update()
{
    // frame time
    m_duration_frame   = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - m_time_begin_frame);
    m_time_begin_frame = std::chrono::steady_clock::now();

    device.draw();
}

void App::on_destroy()
{
    game_running = false;
    // render_thread->join();

    device.destroy();
}
