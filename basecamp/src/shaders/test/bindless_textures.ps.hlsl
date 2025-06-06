struct PS_INPUT
{
    float2 uv : Texcoord0;
    uint texture_id : Texcoord1;
};

// resource
SamplerState Linear_sam;

// bindless has to be placed at the last binding slots, or explicitly declared using the highest slot number eg. register(t0, space1)
Texture2D Textures_srv[]; //: register(t0, space1);
 
float4 main(PS_INPUT input) : SV_Target0
{    
    float3 tex_color = Textures_srv[input.texture_id].Sample(Linear_sam, input.uv).rgb;
    return float4(tex_color, 1.0);

}