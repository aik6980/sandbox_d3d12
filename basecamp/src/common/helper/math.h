#pragma once

struct Vec3 {
    static constexpr XMFLOAT3 Origin = XMFLOAT3(0.0f, 0.0f, 0.0f);
    static constexpr XMFLOAT3 One    = XMFLOAT3(1.0f, 1.0f, 1.0f);
};

struct Quat {
    static constexpr XMFLOAT4 Identity = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
};
