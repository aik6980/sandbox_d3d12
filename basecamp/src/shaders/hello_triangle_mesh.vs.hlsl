
struct PC_vertex
{
    uint vertex_id : SV_VertexID;
    float4 position : Vertex_data0;
    float4 colour : Vertex_data1;
};

struct VS_output
{
    float4 position : SV_Position;
    float3 colour : Colour;
};

VS_output main(PC_vertex input)
{
    VS_output output;
    output.position = float4(input.position.xyz, 1.0);
    output.colour = input.colour.xyz;

    return output;
}