#ifndef DEFAULT_SHARED__HLSLI
#define DEFAULT_SHARED__HLSLI

struct VS_OUTPUT
{
	float4 position : SV_Position;
	float4 colour	: Colour;
};

#define PS_INPUT VS_OUTPUT

cbuffer key
{
	float key_i;
};

#endif
