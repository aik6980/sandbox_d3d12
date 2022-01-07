#ifndef HLSL_SHARED_STRUCT__HLSL
#define HLSL_SHARED_STRUCT__HLSL

#ifdef COMPILE_CPP
#include <DirectXMath.h>
using namespace DirectX;

using float4x4 = XMFLOAT4X4;
#endif

struct Camera_st {
    float4x4 View;
    float4x4 Projection;
};

#endif
