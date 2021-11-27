
const static float2 triangle_positions[3] = {
    float2(0.5, -0.5),
    float2(0.5, 0.5),
    float2(-0.5, 0.5)
    };

const static float3 triangle_colors[3] = {
    float3(1.0, 0.0, 0.0),
    float3(0.0, 1.0, 0.0),
    float3(0.0, 0.0, 1.0)
};


struct VS_OUTPUT
{
    float4 pos      : SV_Position;
    float3 colour   : Colour;
};

VS_OUTPUT main(uint index : SV_VertexID)
{
    VS_OUTPUT output;
    output.pos = float4(triangle_positions[index], 0.0, 1.0);
    output.colour = triangle_colors[index];

    return output;
}