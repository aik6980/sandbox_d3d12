#include "voxelization.hlsl"


VS_output main(VS_input vs_input)
{
    return VS_voxelize(vs_input);

}