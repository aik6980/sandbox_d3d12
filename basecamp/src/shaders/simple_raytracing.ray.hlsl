RaytracingAccelerationStructure Scene;
RWTexture2D<float4> Output;

cbuffer Raygen_cb
{
	float4 Main_vp;
	float4 Stencil_vp;
};


bool Is_inside_viewport(float2 pos, float4 vp)
{
	float left	 = vp.x;
	float top	 = vp.y;
	float right	 = vp.z;
	float bottom = vp.w;

	return (pos.x >= left && pos.x <= right)
		&& (pos.y >= top && pos.y <= bottom);
}

struct Payload_st
{
    float4 colour;
};

[shader("raygeneration")]
void raygen_entry()
{
	uint3 launch_index = DispatchRaysIndex();
	float2 lerp_val = launch_index.xy / (float2)DispatchRaysDimensions();

	// viewport
    float left	= Main_vp.x;
    float top	= Main_vp.y;
    float right = Main_vp.z;
    float bottom= Main_vp.w;

	float3 ray_origin = float3(lerp(left, right, lerp_val.x), lerp(top, bottom, lerp_val.y), 0.0);
	float3 ray_dir = float3(0, 0, 1);

	if (Is_inside_viewport(ray_origin.xy, Stencil_vp)) {
        RayDesc ray;

        ray.Origin = ray_origin;
        ray.Direction = ray_dir;
        ray.TMin      = 0.001;
        ray.TMax      = 10000.0;

		Payload_st payload = { float4(0,0,0,0) };

		uint ray_flags = RAY_FLAG_CULL_BACK_FACING_TRIANGLES;
		uint instance_inclusion_mask = ~0;
		uint ray_contribution_to_hitgroup_index = 0;
		uint multiplier_for_geometry_contribution_to_shader_index = 1;
		uint miss_shader_index = 0;

		TraceRay(Scene, RAY_FLAG_CULL_BACK_FACING_TRIANGLES, instance_inclusion_mask, 
			ray_contribution_to_hitgroup_index,
			multiplier_for_geometry_contribution_to_shader_index,
			miss_shader_index,
			ray,
			payload);

		Output[launch_index.xy] = payload.colour;
	}
    else {
		float3 col              = float3(0.4, 0.6, 0.2);
		Output[launch_index.xy] = float4(col, 1.0);
	}

}

typedef BuiltInTriangleIntersectionAttributes Tri_attributes;

[shader("closesthit")] 
void closethit_entry(inout Payload_st payload, in Tri_attributes attr) 
{
	float3 barycentrics = float3(1.0 - attr.barycentrics.x - attr.barycentrics.y, attr.barycentrics.x, attr.barycentrics.y);
	payload.colour = float4(barycentrics, 1.0);
}

[shader("miss")] 
void miss_entry(inout Payload_st payload)
{
	payload.colour = float4(0,0,0,1);
}

