#include "default_shared.inc.hlsl"

Texture2D		tex_diffuse;
SamplerState	sam_linear;

cbuffer buffer
{
	float4 a;
};

cbuffer cb_ps
{
	float4 colour;
	float4 colour1;
};


float4 main(PS_INPUT input) : SV_Target0
{
	float4 output;
	float4 tex_colour = tex_diffuse.Sample(sam_linear, a.xy);

	float3 colour_tmp = float3(0.0, 1.0, 0.0);
	if (key_i > 0)
	{
		colour_tmp = colour.rgb;
	}
	
	output.rgb = colour_tmp;// +colour1.rgb + tex_colour.rgb;
	output.a = 1.0;
	return output;
}