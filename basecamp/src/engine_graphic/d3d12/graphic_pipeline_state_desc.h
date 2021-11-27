#pragma once

namespace D3D12 {
struct Graphics_pipeline_state_desc : public D3D12_GRAPHICS_PIPELINE_STATE_DESC {
    Graphics_pipeline_state_desc()
    {
        ZeroMemory(this, sizeof(Graphics_pipeline_state_desc));

        RasterizerState       = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
        BlendState            = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
        DepthStencilState     = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
        SampleMask            = UINT_MAX;
        PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
        SampleDesc.Count      = 1;
        SampleDesc.Quality    = 0;
    }

    void set_vertex_shader(ID3DBlob* bytecode)
    {
        VS.pShaderBytecode = bytecode->GetBufferPointer();
        VS.BytecodeLength  = bytecode->GetBufferSize();
    }

    void set_pixel_shader(ID3DBlob* bytecode)
    {
        PS.pShaderBytecode = bytecode ? bytecode->GetBufferPointer() : nullptr;
        PS.BytecodeLength  = bytecode ? bytecode->GetBufferSize() : 0;
    }
};
} // namespace D3D12
