#pragma once

namespace D3D12 {
class ShaderReflection;

struct Shader {
    ComPtr<ID3DBlob>             m_buffer;
    unique_ptr<ShaderReflection> m_reflection;
};

struct Shader_const {
    static const string& Raygen_entry() { return "raygen"; };
    static const string& Miss_entry() { return "miss"; }
    static const string& Closet_hit_entry() { return "closet_hit"; }
};
} // namespace D3D12
