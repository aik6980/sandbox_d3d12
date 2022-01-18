#pragma once

struct Camera {
    enum Projection_mode
    {
        Perspective,
        Ortho,
    };

    Projection_mode m_projection_mode;
    float           m_fov;
    float           m_z_near;
    float           m_z_far;
    float           m_orthographic_height;

    Matrix   m_view;
    XMMATRIX m_projection;
    XMVECTOR m_position;

    XMFLOAT3 position()
    {
        XMFLOAT3 result;
        XMStoreFloat3(&result, m_position);
        return result;
    }

    XMFLOAT4X4 view()
    {
        XMFLOAT4X4 result;
        XMStoreFloat4x4(&result, m_view);
        return result;
    }

    Matrix world()
    {
        auto&& world = m_view.Invert();
        return world;
    }

    XMFLOAT4X4 projection()
    {
        XMFLOAT4X4 result;
        XMStoreFloat4x4(&result, m_projection);
        return result;
    }

    XMFLOAT4X4 projection_to_world()
    {
        auto&& view_proj     = m_view * m_projection;
        auto&& view_proj_inv = XMMatrixInverse(nullptr, view_proj);

        XMFLOAT4X4 result;
        XMStoreFloat4x4(&result, view_proj_inv);
        return result;
    }
};
