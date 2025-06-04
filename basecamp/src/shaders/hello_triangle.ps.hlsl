struct PS_INPUT
{
    float3 colour : Colour;
    float2 uv : Texcoord0;
    uint texture_id : Texcoord1;
};

// resource
Texture2D Textures_srv[];
SamplerState Linear_sam;
 
float4 main(PS_INPUT input) : SV_Target0
{
    //return float4(input.colour, 0.0);
    
    float3 tex_color = Textures_srv[input.texture_id].Sample(Linear_sam, input.uv).rgb;
    return float4(tex_color, 1.0);

}