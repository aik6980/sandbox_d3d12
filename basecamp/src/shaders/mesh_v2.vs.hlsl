#include "mesh.inc.hlsl"
#include "camera.inc.hlsl"

StructuredBuffer<Instance_data_2> Instance_data_srv;
StructuredBuffer<Fat_vertex> Vertices_srv;
Buffer<uint> Indices_srv;

VS_OUTPUT main(uint vertex_id : SV_VertexID, uint instance_id : SV_InstanceID)
{
	// get index 
    uint index = vertex_id;
    Fat_vertex vertex = Vertices_srv[index];
	
	// get instance data
    Instance_data_2 instance_data = Instance_data_srv[instance_id];
	
	float4 pos = float4(vertex.m_position.xyz, 1.0);
    pos = mul(pos, instance_data.m_obj_to_world);
	pos = mul(pos, View);
	pos = mul(pos, Projection);

	VS_OUTPUT output;
	output.position = pos;
	output.colour	= float4(input.colour.rgb, 1.0); //float4(input.colour.xyz, 1.0);

	//float2 uv = pos.xy / pos.w;
	//uv = uv * 0.5 + 0.5;
	//output.uv_coord = float4(uv, 0.0, 0.0);

	//float4 posl = float4(input.position.xyz, 1.0);
    //posl = mul(posl, World);
	//posl = mul(posl, Light_view);
	//posl = mul(posl, Light_projection);
	//output.pos_light = posl;

	return output;
}