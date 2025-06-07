struct PS_INPUT
{
    float4 position : SV_Position;
    float3 colour : Colour;
    float2 uv_coord : Texcoord0;
    uint texture_id : Texcoord1;
};

// resource
SamplerState Linear_sam;

// bindless has to be placed at the last binding slots, or explicitly declared using the highest slot number eg. register(t0, space1)
Texture2D Textures_srv[]; //: register(t0, space1);
 
float4 main(PS_INPUT input) : SV_Target0
{
    return float4(input.colour, 0.0);
    
    //float3 tex_color = Textures_srv[input.texture_id].Sample(Linear_sam, input.uv).rgb;
    //return float4(tex_color, 1.0);

}