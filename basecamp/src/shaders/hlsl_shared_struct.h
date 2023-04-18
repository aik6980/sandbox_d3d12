#ifndef HLSL_SHARED_STRUCT__HLSL
#define HLSL_SHARED_STRUCT__HLSL

#ifdef COMPILE_CPP
#include <DirectXMath.h>
using namespace DirectX;

using float4x4 = XMFLOAT4X4;
using float4   = XMFLOAT4;
using float3   = XMFLOAT3;
using uint	   = uint32_t;

#define Position_sematic
#define Colour_sematic
#else
#define Position_sematic : Position
#define Colour_sematic : Colour
#endif

struct PC_vertex {
	float4 position Position_sematic;
	float4 colour	Colour_sematic;
};

struct Camera_st {
	float4x4 View;
	float4x4 Projection;
};

// using for Raytracing
struct Fat_vertex {
	float3 m_position;
	float4 m_colour;
	float3 m_normal;
};

struct Mesh_desc {
	uint m_num_vertices;
	uint m_num_indices;
	uint m_offset_vertices;
	uint m_offset_indices;
};

struct Instance_data {
	uint m_mesh_id;
};

struct Instance_data_2 {
	float4x4 m_obj_to_world;
};

#endif
