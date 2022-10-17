#include "hlsl_shared_struct.h"

StructuredBuffer<Instance_data> Instance_data_srv;
StructuredBuffer<Mesh_desc> Mesh_data_srv;
StructuredBuffer<Fat_vertex> Vertices_srv;
Buffer<uint> Indices_srv;


bool Is_inside_viewport(float2 pos, float4 vp)
{
	float left	 = vp.x;
	float top	 = vp.y;
	float right	 = vp.z;
	float bottom = vp.w;

	return (pos.x >= left && pos.x <= right)
		&& (pos.y >= top && pos.y <= bottom);
}

inline void Generate_camera_ray(uint2 index, uint2 dimensions, out float3 origin, out float3 dir)
{
    float2 xy = index + 0.5f; // center in the middle of the pixel
    float2 screen_pos = xy / dimensions.xy * 2.0 - 1.0;
	
	// invert y for directx 
    screen_pos.y = -screen_pos.y;

	// unproject the pixel coordinate into a ray
    float4 world_pos = mul(float4(screen_pos, 0.0, 1.0), Camera_projection_to_world);
    world_pos.xyz /= world_pos.w;

    origin = Camera_world_pos;
    dir = normalize(world_pos.xyz - origin);
}

[shader("raygeneration")]
void raygen_entry()
{
	uint3 launch_index = DispatchRaysIndex();
    uint2 dimensions = DispatchRaysDimensions().xy;
    float2 lerp_val = launch_index.xy / (float2) dimensions;

    //Output_uav[launch_index.xy] = float4(1.0, 0.0, 1.0, 1.0);
    //return;

	// viewport
    float left	= Main_vp.x;
    float top	= Main_vp.y;
    float right = Main_vp.z;
    float bottom= Main_vp.w;

	float3 vp_pos = float3(lerp(left, right, lerp_val.x), lerp(top, bottom, lerp_val.y), 0.0);

    if (Is_inside_viewport(vp_pos.xy, Stencil_vp))
    {
		// Generate a ray for a camera pixel corresponding to an index from the dispatched 2D grid.
        float3 ray_origin, ray_dir;
        generate_camera_ray(launch_index.xy, dimensions, ray_origin, ray_dir);

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

        TraceRay(Scene_srv, ray_flags, instance_inclusion_mask,
			ray_contribution_to_hitgroup_index,
			multiplier_for_geometry_contribution_to_shader_index,
			miss_shader_index,
			ray,
			payload);

		Output_uav[launch_index.xy] = payload.colour;
	}
    else {
		float3 col              = float3(0.4, 0.6, 0.2);
        Output_uav[launch_index.xy] = float4(lerp_val, 0.0, 1.0);
    }

}

struct 


[shader("closesthit")] 
void closethit_entry(inout Payload_st payload, in Tri_attributes attr) 
{
	//float3 barycentrics = float3(1.0 - attr.barycentrics.x - attr.barycentrics.y, attr.barycentrics.x, attr.barycentrics.y);
	//payload.colour = float4(barycentrics, 1.0);
    uint instance_index = InstanceIndex();
    uint mesh_id = Instance_data_srv[instance_index].m_mesh_id;
    Mesh_desc mesh_data = Mesh_data_srv[mesh_id];

    // Get the base index of the triangle's first 16 bit index.
    uint index_size_in_bytes = 4;
    uint indices_pre_tri = 3;
    uint base_index = PrimitiveIndex() * indices_pre_tri + mesh_data.m_offset_indices;

    uint3 indices = uint3(Indices_srv[base_index], Indices_srv[base_index + 1], Indices_srv[base_index + 2]);
    indices += mesh_data.m_offset_vertices;
    Fat_vertex vertices[3] = { Vertices_srv[indices.x], Vertices_srv[indices.y], Vertices_srv[indices.z] };


    float3 barycentrics = float3(1.0 - attr.barycentrics.x - attr.barycentrics.y, attr.barycentrics.x, attr.barycentrics.y);
    payload.colour = vertices[0].m_colour * barycentrics.x + vertices[1].m_colour * barycentrics.y + vertices[2].m_colour * barycentrics.z;

    //float3 barycentrics = float3(1.0 - attr.barycentrics.x - attr.barycentrics.y, attr.barycentrics.x, attr.barycentrics.y);
    //payload.colour = float4(barycentrics, 1.0);

}



