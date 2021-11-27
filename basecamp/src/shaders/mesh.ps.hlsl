#include "mesh.inc.hlsl"

Texture2D Diffuse_srv;
SamplerState Point_sampler;

Texture2D Shadow_map_srv;

float4 main(PS_INPUT input) : SV_Target0
{
	float4 tex_col = 1;// Diffuse_srv.Sample(Point_sampler, input.uv_coord.xy);

	float shadow_mul = 1.0;
	if (Receive_shadow > 0)
	{
		// light
		float3 pos_light = input.pos_light.xyz / input.pos_light.w;
		
		if (pos_light.x < -1.0 || pos_light.x > 1.0
			|| pos_light.y < -1.0 || pos_light.y > 1.0
			|| pos_light.z < 0.0 || pos_light.z > 1.0)
		{
			shadow_mul = 1.0;
		}
		else
		{
			// transform clip space to texture space
			pos_light.x = pos_light.x * 0.5 + 0.5;
			pos_light.y = pos_light.y * -0.5 + 0.5;

			float shadow_map_depth = Shadow_map_srv.Sample(Point_sampler, pos_light.xy).r;
			shadow_mul = (pos_light.z < shadow_map_depth) ? 1.0 : 0.1;
		}
	}

	float3 final_col = tex_col.rgb * input.colour.rgb * shadow_mul;
	return float4(final_col, 1.0);
}