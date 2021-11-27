#ifndef CAMERA__INC__HLSL
#define CAMERA__INC__HLSL

cbuffer Camera_cb
{
	float4x4	View;
	float4x4	Projection;
};

#endif