
struct VS_OUTPUT
{
    float4 position : SV_Position;
    float3 colour : Colour;
    float2 uv_coord : Texcoord0;
    uint texture_id : Texcoord1;
};

static const uint Indices[] =
{
    0, 1, 2, 3, 2, 1,
};


// Vulkan Default Normalized Screen Coordinate - Similar to DirectX
// X    -1..1   Left...Right
// Y    -1..1   Top ..Bottom
// Z    0..1    Near.. Far
static const float2 Positions[] =
{
    float2(-0.5,  0.5),
    float2(-0.5, -0.5),
    float2( 0.5,  0.5),
    float2( 0.5, -0.5),
};

static const float2 InstPositions[] =
{
    float2(-0.5, 0.5),
    float2(-0.5, -0.5),
    float2(0.5, 0.5),
    float2(0.5, -0.5),
};

static const float3 InstColours[] =
{
    float3(1.0, 0.0, 0.0),
    float3(0.0, 1.0, 0.0),
    float3(0.0, 0.0, 1.0),
    float3(1.0, 0.0, 1.0),
};

VS_OUTPUT main(uint vertex_id : SV_VertexID, uint instance_id : SV_InstanceID)
{
    VS_OUTPUT output;
    
    uint idx = Indices[vertex_id];
    // local
    float2 pos_local = Positions[idx];
    float scale = 1.0 / 4.0;
    float2 pos_world = pos_local * scale + InstPositions[instance_id];
    
    output.position = float4(pos_world, 0.0, 1.0);
    output.colour = InstColours[instance_id];

    return output;
}