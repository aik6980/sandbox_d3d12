

#include "mesh_novb_renderer.h"

#include "common/common_cpp.h"
#include "engine_graphic/engine_graphic_cpp.h"

#include "app.h"
#include "frame_pipeline.h"
#include "global.h"
#include "world/component/camera.h"

void Mesh_novb_renderer::load_resource()
{
	auto&& device		  = m_engine.render_device();
	auto&& shader_manager = m_engine.shader_mgr();

	{
		m_mesh_novb_technique_handle = "mesh_novb";
		D3D12::TechniqueInit t{.m_vs = "mesh_v2.vs", .m_ps = "mesh.ps"};
		m_engine.shader_mgr().register_technique(m_mesh_novb_technique_handle, t);
	}
}

void Mesh_novb_renderer::draw_meshes()
{
}

void Mesh_novb_renderer::draw()
{
}
