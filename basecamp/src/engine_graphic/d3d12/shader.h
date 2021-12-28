#pragma once

namespace D3D12 {
class ShaderReflection;
class Lib_ray_reflection;

struct Shader {
    ComPtr<ID3DBlob>             m_buffer;
    unique_ptr<ShaderReflection> m_reflection;
};

struct Lib_ray_shader {
    ComPtr<ID3DBlob>               m_buffer;
    unique_ptr<Lib_ray_reflection> m_reflection;
};

} // namespace D3D12
