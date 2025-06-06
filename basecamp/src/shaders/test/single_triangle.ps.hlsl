struct PS_INPUT
{
    float3 colour : Colour;
};
 
float4 main(PS_INPUT input) : SV_Target0
{
    return float4(input.colour, 0.0);
}