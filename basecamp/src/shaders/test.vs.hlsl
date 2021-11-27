#include "default_shared.inc.hlsl"

struct VS_INPUT
{
	float3 position : POSITION;
	float4 colour	: COLOUR;
	float2 uv		: TEXTURE;
};

VS_OUTPUT main(VS_INPUT input)
{
	VS_OUTPUT output;
	output.position = float4(input.position.xyz, 1.0);

	return output;
}