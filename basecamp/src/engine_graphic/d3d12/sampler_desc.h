#pragma once

namespace D3D12 {

struct D3d12x_sampler_desc : public D3D12_SAMPLER_DESC {
    explicit D3d12x_sampler_desc(const D3D12_SAMPLER_DESC& o) noexcept : D3D12_SAMPLER_DESC(o) {}
    D3d12x_sampler_desc(D3D12_FILTER filter = D3D12_FILTER_ANISOTROPIC, D3D12_TEXTURE_ADDRESS_MODE addressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP,
        D3D12_TEXTURE_ADDRESS_MODE addressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP, D3D12_TEXTURE_ADDRESS_MODE addressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP,
        FLOAT mipLODBias = 0, UINT maxAnisotropy = 16, D3D12_COMPARISON_FUNC comparisonFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL, FLOAT minLOD = 0.f,
        FLOAT maxLOD = D3D12_FLOAT32_MAX) noexcept
    {
        Init(filter, addressU, addressV, addressW, mipLODBias, maxAnisotropy, comparisonFunc, minLOD, maxLOD);
    }

    static inline void Init(_Out_ D3D12_SAMPLER_DESC& samplerDesc, D3D12_FILTER filter = D3D12_FILTER_ANISOTROPIC,
        D3D12_TEXTURE_ADDRESS_MODE addressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP, D3D12_TEXTURE_ADDRESS_MODE addressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP,
        D3D12_TEXTURE_ADDRESS_MODE addressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP, FLOAT mipLODBias = 0, UINT maxAnisotropy = 16,
        D3D12_COMPARISON_FUNC comparisonFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL, FLOAT minLOD = 0.f, FLOAT maxLOD = D3D12_FLOAT32_MAX) noexcept
    {
        samplerDesc                = {};
        samplerDesc.Filter         = filter;
        samplerDesc.AddressU       = addressU;
        samplerDesc.AddressV       = addressV;
        samplerDesc.AddressW       = addressW;
        samplerDesc.MipLODBias     = mipLODBias;
        samplerDesc.MaxAnisotropy  = maxAnisotropy;
        samplerDesc.ComparisonFunc = comparisonFunc;
        // C-array :( need to come back and see how should I write this
        // samplerDesc.BorderColor    = nullptr;
        samplerDesc.MinLOD = minLOD;
        samplerDesc.MaxLOD = maxLOD;
    }
    inline void Init(D3D12_FILTER filter = D3D12_FILTER_ANISOTROPIC, D3D12_TEXTURE_ADDRESS_MODE addressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP,
        D3D12_TEXTURE_ADDRESS_MODE addressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP, D3D12_TEXTURE_ADDRESS_MODE addressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP,
        FLOAT mipLODBias = 0, UINT maxAnisotropy = 16, D3D12_COMPARISON_FUNC comparisonFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL, FLOAT minLOD = 0.f,
        FLOAT maxLOD = D3D12_FLOAT32_MAX) noexcept
    {
        Init(*this, filter, addressU, addressV, addressW, mipLODBias, maxAnisotropy, comparisonFunc, minLOD, maxLOD);
    }
};

} // namespace D3D12
