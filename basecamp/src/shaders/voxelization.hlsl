#include "hlsl_shared_struct.h"

typedef PC_vertex VS_input;

struct VS_output
{
    float4 pos : SV_Position;
    float4 grid_pos : Position_grid;
};

typedef VS_output PS_input;

cbuffer Cb_vs
{
    float4x4 world_to_projection;
    float4x4 world_to_voxel;
};


VS_output VS_voxelize(VS_input input)
{
    VS_output output;

	// transform to clip space
    output.pos = input.position; //mul(g_matModelToProj, input.pos);

	// transform to voxel space
    output.grid_pos = input.position; //mul(g_matModelToVoxel, input.pos);

    return output;
}

float4 PS_voxelize_surface(PS_input input)
{
    return float4(1.0, 1.0, 1.0, 1.0);

}
