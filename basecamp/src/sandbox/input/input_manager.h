#pragma once

class Input_event_handler : public OIS::KeyListener, public OIS::MouseListener, public OIS::JoyStickListener {
    bool keyPressed(const OIS::KeyEvent &arg) override
    {
        return true;
    }
    bool keyReleased(const OIS::KeyEvent &arg) override
    {
        return true;
    }
};

class Input_manager {
  public:
    void init(HWND hwnd);
    void destroy();
    void update();

    bool is_keydown(OIS::KeyCode key)
    {
        return m_keyboard->isKeyDown(key);
    }

  private:
    OIS::InputManager *m_input_manager = nullptr;
    // device
    OIS::Keyboard *m_keyboard = nullptr;
    OIS::Mouse *m_mouse = nullptr;

    static const int Max_numjoy = 4;
    std::array<OIS::JoyStick *, Max_numjoy> m_joystick = {nullptr};

    static const bool Create_input_buffer_mode = false;
    // Input_event_handler m_handler;
};
