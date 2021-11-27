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

    XMMATRIX m_view;
    XMMATRIX m_projection;

    XMFLOAT4X4 view()
    {
        XMFLOAT4X4 result;
        XMStoreFloat4x4(&result, m_view);
        return result;
    }

    XMFLOAT4X4 projection()
    {
        XMFLOAT4X4 result;
        XMStoreFloat4x4(&result, m_projection);
        return result;
    }
};
