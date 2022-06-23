#include "voxelization.hlsl"


float4 main(PS_input input) : SV_Target0
{
    return PS_voxelize_surface(input);

}