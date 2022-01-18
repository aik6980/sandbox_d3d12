#include "input_manager.h"

void Input_manager::init(HWND hwnd)
{
    OIS::ParamList    pl;
    std::stringstream wnd;
    wnd << (size_t)hwnd;

    pl.insert(std::make_pair("WINDOW", wnd.str()));
    pl.insert(std::make_pair(std::string("w32_mouse"), std::string("DISCL_FOREGROUND")));
    pl.insert(std::make_pair(std::string("w32_mouse"), std::string("DISCL_NONEXCLUSIVE")));
    pl.insert(std::make_pair(std::string("w32_keyboard"), std::string("DISCL_FOREGROUND")));
    pl.insert(std::make_pair(std::string("w32_keyboard"), std::string("DISCL_NONEXCLUSIVE")));

    m_input_manager = OIS::InputManager::createInputSystem(pl);
    m_input_manager->enableAddOnFactory(OIS::InputManager::AddOn_All);

    // Print debugging information
    uint32_t v = m_input_manager->getVersionNumber();
    std::cout << "OIS Version: " << (v >> 16) << "." << ((v >> 8) & 0x000000FF) << "." << (v & 0x000000FF)
              << "\nRelease Name: " << m_input_manager->getVersionName() << "\nManager: " << m_input_manager->inputSystemName()
              << "\nTotal Keyboards: " << m_input_manager->getNumberOfDevices(OIS::OISKeyboard)
              << "\nTotal Mice: " << m_input_manager->getNumberOfDevices(OIS::OISMouse)
              << "\nTotal JoySticks: " << m_input_manager->getNumberOfDevices(OIS::OISJoyStick);

    // List all devices
    const char* device_type[6] = {"OISUnknown", "OISKeyboard", "OISMouse", "OISJoyStick", "OISTablet", "OISOther"};

    OIS::DeviceList list = m_input_manager->listFreeDevices();
    for (OIS::DeviceList::iterator i = list.begin(); i != list.end(); ++i)
        std::cout << "\n\tDevice: " << device_type[i->first] << " Vendor: " << i->second;

    m_keyboard = (OIS::Keyboard*)m_input_manager->createInputObject(OIS::OISKeyboard, Create_input_buffer_mode);
    // m_keyboard->setEventCallback(&handler);

    m_mouse = (OIS::Mouse*)m_input_manager->createInputObject(OIS::OISMouse, Create_input_buffer_mode);
    // m_mouse->setEventCallback(&handler);
    const OIS::MouseState& ms = m_mouse->getMouseState();
    // set window info
    RECT rect;
    if (GetClientRect(hwnd, &rect)) {
        ms.width  = rect.right;
        ms.height = rect.bottom;
    }
    m_curr_mouse_state = m_prev_mouse_state = ms;

    try {
        // This demo uses at most 4 joysticks - use old way to create (i.e.
        // disregard vendor)
        int num_sticks = std::min(m_input_manager->getNumberOfDevices(OIS::OISJoyStick), Max_numjoy);
        for (int i = 0; i < num_sticks; ++i) {
            m_joystick[i] = (OIS::JoyStick*)m_input_manager->createInputObject(OIS::OISJoyStick, Create_input_buffer_mode);
            // g_joys[i]->setEventCallback(&handler);
            std::cout << "\n\nCreating Joystick " << (i + 1) << "\n\tAxes: " << m_joystick[i]->getNumberOfComponents(OIS::OIS_Axis)
                      << "\n\tSliders: " << m_joystick[i]->getNumberOfComponents(OIS::OIS_Slider)
                      << "\n\tPOV/HATs: " << m_joystick[i]->getNumberOfComponents(OIS::OIS_POV)
                      << "\n\tButtons: " << m_joystick[i]->getNumberOfComponents(OIS::OIS_Button)
                      << "\n\tVector3: " << m_joystick[i]->getNumberOfComponents(OIS::OIS_Vector3);
        }
    }
    catch (OIS::Exception& ex) {
        std::cout << "\nException raised on joystick creation: " << ex.eText << std::endl;
    }
}

void Input_manager::destroy()
{
    // Destroying the manager will cleanup unfreed devices
    std::cout << "OIS Cleaning up...\n";
    if (m_input_manager) {
        OIS::InputManager::destroyInputSystem(m_input_manager);
    }
}

void Input_manager::update()
{
    if (m_keyboard) {
        m_keyboard->capture();
        // if (!m_keyboard->buffered()) {
        //   // logic here
        // }
    }

    if (m_mouse) {
        m_mouse->capture();

        m_prev_mouse_state = m_curr_mouse_state;
        m_curr_mouse_state = m_mouse->getMouseState();

        // DBG::OutputString("%d %d %d \n", m_curr_mouse_state.X.abs, m_curr_mouse_state.Y.abs, m_curr_mouse_state.buttonDown(OIS::MouseButtonID::MB_Left));

        // if (!m_mouse->buffered()) {
        //   // logic here
        // }
    }

    for (int i = 0; i < 4; ++i) {
        if (m_joystick[i]) {
            m_joystick[i]->capture();
            if (!m_joystick[i]->buffered()) {
                // logic
            }
        }
    }
}

Vector2 Input_manager::mouse_npos() const
{
    float x = (float)m_curr_mouse_state.X.abs / m_curr_mouse_state.width * 2.0f - 1.0f;
    float y = 1.0f - (float)m_curr_mouse_state.Y.abs / m_curr_mouse_state.height * 2.0f;

    return Vector2(x, y);
}
