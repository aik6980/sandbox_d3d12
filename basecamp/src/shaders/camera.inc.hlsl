#ifndef CAMERA__INC__HLSL
#define CAMERA__INC__HLSL

#include "hlsl_shared_struct.h"

cbuffer Camera_cb
{
	float4x4	View;
	float4x4	Projection;
};

ConstantBuffer<Camera_st> Camera_cb_v51;

#endif