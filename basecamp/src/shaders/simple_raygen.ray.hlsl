RaytracingAccelerationStructure Rt_scene;
RWTexture2D<float4> Output;

[shader("raygeneration")]
void raygen()
{
	uint3 launch_index = DispatchRaysIndex();
	float3 col = float3(0.4, 0.6, 0.2);

	Output[launch_index.xy] = float4(col, 1.0);
}


struct Payload_struct
{
	bool hit;
};

[shader("miss")]
void miss(inout Payload_struct payload)
{
	payload.hit = false;
}

