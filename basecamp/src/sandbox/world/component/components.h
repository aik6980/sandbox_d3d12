#pragma once

#include "common/common.h"

struct Transform {
    XMFLOAT3 m_pos    = Vec3::Origin;
    XMFLOAT4 m_orient = Quat::Identity;
    XMFLOAT3 m_scale  = Vec3::One;
};

struct Player {
};

struct Light {

};