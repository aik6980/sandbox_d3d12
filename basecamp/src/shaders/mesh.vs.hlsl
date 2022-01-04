#include "mesh.inc.hlsl"
#include "camera.inc.hlsl"

struct VS_INPUT
{
	float3 position : Position;
	float3 colour	: Colour;
};

cbuffer Object_cb
{
	float4x4 World;
};
//ConstantBuffer<Object_struct> Object_cb;

VS_OUTPUT main(VS_INPUT input)
{
	float4 pos = float4(input.position.xyz, 1.0);
	pos = mul(pos, World);
	pos = mul(pos, View);
	pos = mul(pos, Projection);

	VS_OUTPUT output;
	output.position = pos;
	output.colour	= float4(input.colour.rgb, 1.0); //float4(input.colour.xyz, 1.0);

	//float2 uv = pos.xy / pos.w;
	//uv = uv * 0.5 + 0.5;
	//output.uv_coord = float4(uv, 0.0, 0.0);

	float4 posl = float4(input.position.xyz, 1.0);
	posl = mul(posl, World);
	posl = mul(posl, Light_view);
	posl = mul(posl, Light_projection);
	output.pos_light = posl;

	return output;
}