#pragma once

class Input_manager;
class Engine;

class App {
  public:
    static Engine&         engine() { return *m_engine; }
    static entt::registry& entt_reg() { return m_reg; }

    static Input_manager& input() { return *m_input_manager; }

    static float get_duration_app()
    {
        return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - m_time_begin_app).count() * 1e-6f;
    }

    static float get_duration_frame() { return m_duration_frame.count() * (float)1e-6; }

    void on_init(HINSTANCE hInstance, HWND hWnd);
    void on_update();
    void on_destroy();

    void on_event_msg(const MSG msg){};

    void create_world();

  private:
    HINSTANCE m_hInstance;
    HWND      m_hWnd;

    static std::unique_ptr<Engine>        m_engine;
    static std::unique_ptr<Input_manager> m_input_manager;
    static entt::registry                 m_reg;

    static std::chrono::time_point<std::chrono::steady_clock> m_time_begin_app;
    static std::chrono::time_point<std::chrono::steady_clock> m_time_begin_frame;
    static std::chrono::microseconds                          m_duration_frame;
};
