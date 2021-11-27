#ifndef MESH__HLSLI
#define MESH__HLSLI

struct VS_OUTPUT
{
	float4 position : SV_Position;
	float4 colour	: Colour;
	float4 uv_coord	: Texcoord0;
	float4 pos_light : Texcoord1;
};

#define PS_INPUT VS_OUTPUT

cbuffer Light_cb
{
	float4x4 Light_view;
	float4x4 Light_projection;
	int		 Receive_shadow;
};

#endif