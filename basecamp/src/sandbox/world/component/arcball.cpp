#include "arcball.h"

#include "app.h"
#include "input/input_manager.h"

void Arcball::init(const Vector3& pos, const Vector3& target)
{
    m_pos        = pos;
    m_target     = target;
    m_zoom       = (target - pos).Length();
    Matrix view  = XMMatrixLookAtLH(pos, target, Vector3::Up); // CreateLookAtLH(pos, target, Vector3::Up);
    auto&& world = view.Invert();
    m_orient     = Quaternion::CreateFromRotationMatrix(world);
}

void Arcball::update()
{
    switch (m_state) {
    case State_idle: {
        if (App::input().mouse_down(OIS::MouseButtonID::MB_Left)) {
            m_state = State_rotate_arcball;

            m_begin_mouse_pos = npos_to_vector(App::input().mouse_npos());
            m_begin_orient    = m_orient;
        }
    } break;
    case State_rotate_arcball: {
        if (App::input().mouse_down(OIS::MouseButtonID::MB_Left) == false) {
            m_state = State_idle;
        }

        auto&& mouse_pos = npos_to_vector(App::input().mouse_npos());
        // find rot quaternion from point a to point b on the unit sphere
        Quaternion rot;
        {
            // https://stackoverflow.com/questions/1171849/finding-quaternion-representing-the-rotation-from-one-vector-to-another
            // Quaternion q;
            // vector a = crossproduct(v1, v2);
            // q.xyz = a;
            // q.w = sqrt((v1.Length ^ 2) * (v2.Length ^ 2)) + dotproduct(v1, v2);
            auto&&  v_from = m_begin_mouse_pos;
            auto&&  v_to   = mouse_pos;
            Vector3 cross  = v_from.Cross(v_to);
            float   dot    = v_from.Dot(v_to);

            rot = Quaternion(cross.x, cross.y, cross.z, dot);
        }

        m_orient = m_begin_orient * rot;
        Matrix m = Matrix::CreateFromQuaternion(m_orient);
        m_pos    = m_target + m.Forward() * m_zoom;

        // Vector2 delta_angle = (mouse_pos - m_prev_mouse_pos);
        // delta_angle.x *= XM_2PI;
        // delta_angle.y *= XM_PI;

        // find new camera position
        // auto&& rot = Matrix::CreateFromYawPitchRoll(delta_angle.x, delta_angle.y, 0.0f);
        // auto&& rot = Quaternion::CreateFromYawPitchRoll(delta_angle.x, delta_angle.y, 0.0f);
        // m_pos      = Vector3::Transform(m_pos, rot);
        //// find new orientation
        // auto&& view  = Matrix::CreateLookAtLH(m_pos, m_target, Vector3::Up);
        // auto&& world = view.Invert();
        // m_orient     = Quaternion::CreateFromRotationMatrix(world);
        //
        //// store state
        // m_prev_mouse_pos = mouse_pos;
    } break;
    }
}

Vector3 Arcball::npos_to_vector(const Vector2& npos)
{
    // adjust the arcball size inscreen space
    Vector2 pos = npos * 1.0f / m_radius_screen_space;

    // calculate surface position of unit sphere from 2d pos
    float z    = 0.0f;
    float l_sq = pos.LengthSquared();

    if (l_sq > 1.0f) {
        float scale = 1.0f / sqrtf(l_sq);
        pos *= scale;
    }
    else {
        z = sqrtf(1.0f - l_sq);
    }

    return Vector3(pos.x, pos.y, z);
}

Matrix Arcball::view()
{
    // auto&& cam_rot = Matrix::CreateFromQuaternion(m_orient).Invert();
    // auto&& view = XMMatrixLookAtLH(m_pos, m_target, cam_rot.Up());

    auto&& world = Matrix::CreateFromQuaternion(m_orient);
    world.Translation(m_pos);
    auto&& view = world.Invert();
    return view;
}
