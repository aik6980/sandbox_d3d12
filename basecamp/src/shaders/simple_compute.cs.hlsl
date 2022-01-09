
RWTexture2D<float4> Texture_uav;

Texture2D Texture_srv;

[numthreads(32, 32, 1)]
void main(uint3 Gid : SV_GroupID,
	uint3 DTid : SV_DispatchThreadID,
	uint3 GTid : SV_GroupThreadID,
	uint GI : SV_GroupIndex)
{
    uint w, h;
    Texture_uav.GetDimensions(w, h);

    if (DTid.x >= w || DTid.y >= h)
    {
        return;
    }

    if (DTid.x > w * 0.51)
    {
        float4 col = Texture_srv.Load(int3(DTid.xy, 0));
        Texture_uav[DTid.xy] = col;
        return;
    }
    else if (DTid.x > w * 0.49)
    {
        float4 col = Texture_uav[DTid.xy];
        float lum = col.r * 0.3 + col.g * 0.59 + col.b * 0.11;
    
        Texture_uav[DTid.xy] = float4(lum.xxx, 1.0);
        return;
    }
}