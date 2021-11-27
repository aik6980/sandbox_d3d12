RWTexture2D<float4> output_texture;

cbuffer cb
{
    float4 color;
    row_major float4x4 mat_world;
};
    
cbuffer cb1
{
    float4 color1;   
};

[numthreads(32, 32, 1)]
void main(uint3 Gid : SV_GroupID,
	uint3 DTid : SV_DispatchThreadID,
	uint3 GTid : SV_GroupThreadID,
	uint GI : SV_GroupIndex)
{
    float4 final_col = mul(color, mat_world);
    if(GI % 2 == 0) final_col = color1;
    
    output_texture[DTid.xy] = final_col;
}