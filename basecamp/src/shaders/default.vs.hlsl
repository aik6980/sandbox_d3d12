#include "default_shared.inc.hlsl"

static const float3 tri[] = 
{
	float3( 0.0, 0.5, 0.5),
	float3( 0.5, -0.5, 0.5),
	float3(-0.5, -0.5, 0.5)
};

cbuffer cb_vs
{
	float2 offset;
	float2 scale;
	float  offsety;
};

VS_OUTPUT main(uint vertex_id : SV_VertexID)
{
	VS_OUTPUT output;

	float2 offset1 = float2(0, 0.25);
	if (key_i > 0)
	{
		offset1 = offset;
	}

	float2 pos = tri[vertex_id].xy * scale + offset1;
	pos.y += offsety;

	output.position = float4(pos, tri[vertex_id].z, 1.0);

	return output;
}
