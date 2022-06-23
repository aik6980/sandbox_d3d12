#pragma once

class Arcball {
  public:
    void init(const Vector3& pos, const Vector3& target);
    void update();

    Vector3 npos_to_vector(const Vector2& npos);

    Vector3 pos();
    Matrix  view();

    enum State
    {
        State_idle,
        State_rotate_arcball,
    };

    State m_state = State_idle;

    Vector2    m_begin_mouse_pos;
    Quaternion m_begin_orient;

    Vector3    m_target;
    Quaternion m_orient;
    float      m_radius_screen_space = 0.9f; // arcball's radius in screen space;
    float      m_zoom;

    Vector3 m_view, m_right;
};
