
struct VS_OUTPUT
{
    float4 position : SV_Position;
    float2 uv_coord : Texcoord0;
    uint texture_id : Texcoord1;
};

static const uint Indices[] =
{
    0, 1, 2, 3, 2, 1,
};


static const float2 VPosition[] =
{
    float2(0.0, -0.5),
    float2(0.5, 0.5),
    float2(-0.5, 0.5),
    float2(-0.5, -0.5),
};

static const float2 UvCoords[] =
{
    float2(0.0, -0.5),
    float2(0.5, 0.5),
    float2(-0.5, 0.5),
    float2(-0.5, -0.5),
};

static const float2 InstPositions[] =
{
    float2(0.0, -0.5),
    float2(0.5, 0.5),
    float2(-0.5, 0.5),
    float2(-0.5, -0.5),
};

VS_OUTPUT main(uint vertex_id : SV_VertexID)
{
    VS_OUTPUT output;
    
    output.position = float4(VPosition[vertex_id], 0.0, 1.0);

    return output;
}