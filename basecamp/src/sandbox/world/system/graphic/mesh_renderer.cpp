

#include "mesh_renderer.h"

#include "app.h"
#include "common/common_cpp.h"
#include "engine_graphic/engine_graphic_cpp.h"
#include "frame_pipeline.h"
#include "global.h"
#include "world/component/camera.h"

void Mesh_renderer::load_resource()
{
	auto&& device		  = m_engine.render_device();
	auto&& shader_manager = m_engine.shader_mgr();

	{
		string				 technique_name = "default";
		D3D12::TechniqueInit t{.m_vs = "default.vs", .m_ps = "default.ps"};
		m_engine.shader_mgr().register_technique(technique_name, t);
	}

	{
		m_technique_mesh = "mesh";
		D3D12::TechniqueInit t{.m_vs = "mesh.vs", .m_ps = "mesh.ps"};
		m_engine.shader_mgr().register_technique(m_technique_mesh, t);
	}

	{
		m_technique_shadow_map = "mesh_shadow_map";
		D3D12::TechniqueInit t{.m_vs = "mesh.vs", .m_ps = ""};
		m_engine.shader_mgr().register_technique(m_technique_shadow_map, t);
	}

	{
		m_technique_mesh_instancing = "mesh_instancing";
		D3D12::TechniqueInit t{.m_vs = "mesh_instancing.vs", .m_ps = "mesh.ps"};
		m_engine.shader_mgr().register_technique(m_technique_mesh_instancing, t);
	}

	// build a mesh
	// Model_loader model_loader;
	// model_loader.load("..\\assets\\DuckWhite.fbx");
	//{
	//    auto&& mesh_data  = model_loader.meshes();
	//    auto&& mesh_verts = MeshDataGenerator::to_p1c1(mesh_data.m_vertices);
	//    m_duck_white_mesh = build_mesh(mesh_verts, mesh_data.m_indices, "duck_white_mesh", m_engine);
	//}

	// build_quad_mesh();
	// build_cube_mesh();

	// auto&& mesh_data  = MeshDataGenerator::create_grid(25.0, 25.0, 10, 10);
	// auto&& mesh_verts = MeshDataGenerator::to_p1c1(mesh_data.m_vertices);
	// m_grid_mesh       = build_mesh(mesh_verts, mesh_data.m_indices, "grid_mesh", m_engine);

	build_texture();

	// create an instance buffer
	m_instance_data.resize(m_num_instances);
	m_engine.resource_mgr().create_instance_buffer(m_instance_data_buffer_name, sizeof(Instance_data) * m_num_instances);

	// load assets
	for (auto&& kvp : m_frame_pipeline.get_mesh_data_pool()) {
		auto&& mesh_data  = kvp.second;
		auto&& mesh_verts = MeshDataGenerator::to_p1c1(mesh_data->m_vertices);
		build_mesh(mesh_verts, mesh_data->m_indices, kvp.first, m_engine);
	}
}

void Mesh_renderer::draw_meshes_shadow_map()
{
	auto&& camera = m_light;

	auto&& rt_fmt = m_frame_pipeline.m_render_pass_shadow_map->render_target_format();
	auto&& ds_fmt = m_frame_pipeline.m_render_pass_shadow_map->depth_stencil_format();

	auto&& command_list = m_engine.render_device().commmand_list();

	// auto&& mesh_buffer = m_engine.resource_mgr().request_mesh_buffer(m_unit_cube_name).lock();
	auto&& mesh_buffer = m_engine.resource_mgr().request_mesh_buffer("").lock();

	auto&& technique_instance = m_engine.shader_mgr().create_technique_instance(m_technique_shadow_map, rt_fmt, ds_fmt);
	if (technique_instance && mesh_buffer) {

		technique_instance->set_cbv("Camera_cb", "View", camera.view());
		technique_instance->set_cbv("Camera_cb", "Projection", camera.projection());

		XMMATRIX world = XMMatrixTranslation(0.0f, 1.0f, 0.0f);
		technique_instance->set_cbv("Object_cb", "World", &world, sizeof(world));

		auto&& tex = m_engine.resource_mgr().request_buffer(m_texture_name);
		technique_instance->set_srv("Diffuse_srv", tex);

		command_list()->SetPipelineState(technique_instance->m_pso.Get());
		technique_instance->set_root_signature_parameters(*command_list());

		auto&& vbv = mesh_buffer->vertex_buffer_view();
		command_list()->IASetVertexBuffers(0, 1, &vbv);
		auto&& ibv = mesh_buffer->index_buffer_view();
		command_list()->IASetIndexBuffer(&ibv);
		command_list()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		auto&& index_count = mesh_buffer->m_mesh_location.index_count;
		command_list()->DrawIndexedInstanced(index_count, 1, 0, 0, 0);
	}
}

void Draw_indexed_mesh(D3D12::Command_list& command_list, D3D12::Technique_instance& technique_instance, D3D12::Mesh_buffer& mesh_buffer)
{
	auto&& vbv = mesh_buffer.vertex_buffer_view();
	command_list()->IASetVertexBuffers(0, 1, &vbv);
	auto&& ibv = mesh_buffer.index_buffer_view();
	command_list()->IASetIndexBuffer(&ibv);
	command_list()->IASetPrimitiveTopology(mesh_buffer.prim_topology);

	auto&& index_count = mesh_buffer.m_mesh_location.index_count;
	command_list()->DrawIndexedInstanced(index_count, 1, 0, 0, 0);
}

void Mesh_renderer::draw_meshes()
{
	auto&& cam = m_frame_pipeline.m_camera;

	auto&& rt_fmt = m_frame_pipeline.m_render_pass_main->render_target_format();
	auto&& ds_fmt = m_frame_pipeline.m_render_pass_main->depth_stencil_format();

	auto&& render_device = m_engine.render_device();
	auto&& command_list	 = m_engine.render_device().commmand_list();

	// auto&& mesh_buffer = m_engine.resource_mgr().request_mesh_buffer(m_unit_cube_name).lock();
	auto&& mesh_buffer = m_engine.resource_mgr().request_mesh_buffer("").lock();

	// auto&& duck_mesh = m_engine.resource_mgr().request_mesh_buffer(m_duck_white_mesh).lock();
	auto&& duck_mesh = m_engine.resource_mgr().request_mesh_buffer("").lock();

	// auto&& mesh_tech_handle = m_render_technique_mesh_instance->get_technique();
	// auto&& pso              = m_engine.shader_mgr().get_pso(mesh_tech_handle, rt_fmt, ds_fmt);
	// if (pso && mesh_buffer)
	{
		auto&& technique_instance = m_engine.shader_mgr().create_technique_instance(m_technique_mesh, rt_fmt, ds_fmt);
		if (technique_instance && duck_mesh) {

			technique_instance->set_cbv("Camera_cb", "View", cam.view());
			technique_instance->set_cbv("Camera_cb", "Projection", cam.projection());

			technique_instance->set_cbv("Light_cb", "Receive_shadow", 0);

			auto&& world = Matrix::CreateScale(0.1f) * Matrix::CreateTranslation(0.0f, 0.1f, 0.0f);
			// auto&& world = m_frame_pipeline.m_test_world;
			technique_instance->set_cbv("Object_cb", "World", &world, sizeof(world));

			auto&& tex = m_engine.resource_mgr().request_buffer(m_texture_name);
			technique_instance->set_srv("Diffuse_srv", tex);

			command_list()->SetPipelineState(technique_instance->m_pso.Get());
			technique_instance->set_root_signature_parameters(*command_list());

			Draw_indexed_mesh(command_list, *technique_instance, *duck_mesh);
		}
	}

	{
		auto&& technique_instance = m_engine.shader_mgr().create_technique_instance(m_technique_mesh_instancing, rt_fmt, ds_fmt);
		if (technique_instance && mesh_buffer) {

			// m_mesh_instancing_technique_instance->set_cbv("Camera_cb", "View", &cam.view(), sizeof(cam.view()));
			// m_mesh_instancing_technique_instance->set_cbv("Camera_cb", "Projection", &cam.projection(), sizeof(cam.projection()));

			Camera_st camera_cb = {cam.view(), cam.projection()};
			technique_instance->set_cbv("Camera_cb_v51", "Camera_cb_v51", camera_cb);
			technique_instance->set_cbv("Light_cb", "Receive_shadow", 0);

			auto&& tex = m_engine.resource_mgr().request_buffer(m_texture_name);
			technique_instance->set_srv("Diffuse_srv", tex);

			// update instance data
			m_engine.resource_mgr().update_dynamic_buffer(
				m_instance_data_buffer_name, m_instance_data.data(), sizeof(Instance_data) * (uint32_t)m_instance_data.size());

			command_list()->SetPipelineState(technique_instance->m_pso.Get());
			technique_instance->set_root_signature_parameters(*command_list());

			std::array<D3D12_VERTEX_BUFFER_VIEW, 2> vb_views = {
				mesh_buffer->vertex_buffer_view(), m_engine.resource_mgr().request_instance_buffer_view(m_instance_data_buffer_name, sizeof(Instance_data))};

			command_list()->IASetVertexBuffers(0, (uint32_t)vb_views.size(), vb_views.data());
			auto&& ibv = mesh_buffer->index_buffer_view();
			command_list()->IASetIndexBuffer(&ibv);
			command_list()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

			auto&& index_count = mesh_buffer->m_mesh_location.index_count;
			command_list()->DrawIndexedInstanced(index_count, (uint32_t)m_instance_data.size(), 0, 0, 0);
		}
	}

	// auto&& grid_mesh_buffer = m_engine.resource_mgr().request_mesh_buffer(m_grid_mesh).lock();

	auto&& scene_container = m_frame_pipeline.m_scene_container;
	for (auto&& kvp : scene_container.m_scene_instances) {

		auto&& mesh_buffer		  = m_engine.resource_mgr().request_mesh_buffer(kvp.first).lock();
		auto&& technique_instance = m_engine.shader_mgr().create_technique_instance(m_technique_mesh, rt_fmt, ds_fmt);

		if (mesh_buffer && technique_instance) {

			technique_instance->set_cbv("Camera_cb", "View", cam.view());
			technique_instance->set_cbv("Camera_cb", "Projection", cam.projection());

			technique_instance->set_cbv("Light_cb", "Receive_shadow", 0);
			technique_instance->set_cbv("Light_cb", "Light_view", m_light.view());
			technique_instance->set_cbv("Light_cb", "Light_projection", m_light.projection());
			auto&& shadow_map_tex = m_frame_pipeline.m_render_pass_shadow_map->depth_stencil_buffer();
			technique_instance->set_srv("Shadow_map_srv", shadow_map_tex);

			auto&& sampler = m_engine.resource_mgr().request_sampler(m_point_sampler);
			technique_instance->set_sampler("Point_sampler", sampler);

			XMMATRIX world = kvp.second.transform;
			technique_instance->set_cbv("Object_cb", "World", &world, sizeof(world));

			auto&& tex = m_engine.resource_mgr().request_buffer(m_texture_name);
			technique_instance->set_srv("Diffuse_srv", tex);

			command_list()->SetPipelineState(technique_instance->m_pso.Get());
			technique_instance->set_root_signature_parameters(*command_list());

			auto&& vbv = mesh_buffer->vertex_buffer_view();
			command_list()->IASetVertexBuffers(0, 1, &vbv);
			auto&& ibv = mesh_buffer->index_buffer_view();
			command_list()->IASetIndexBuffer(&ibv);
			command_list()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

			auto&& index_count = mesh_buffer->m_mesh_location.index_count;
			command_list()->DrawIndexedInstanced(index_count, 1, 0, 0, 0);
		}
	}
}

void Mesh_renderer::update()
{
	ImGui::Begin("Rasterizer debug");

	{
		auto&& tex = m_engine.resource_mgr().request_buffer("raytrace_colour_buffer").lock();
		if (tex) {
			auto   w   = tex->m_d3d_desc.Width * 0.5f;
			auto   h   = tex->m_d3d_desc.Height * 0.5f;
			auto&& srv = m_engine.resource_mgr().create_srv(*tex);
			ImGui::Image(reinterpret_cast<ImTextureID>(srv.ptr), ImVec2(w, h));
		}
	}

	auto&& tex = m_engine.resource_mgr().request_buffer("checkerboard_texture").lock();
	if (tex) {
		auto   w   = tex->m_d3d_desc.Width * 0.5f;
		auto   h   = tex->m_d3d_desc.Height * 0.5f;
		auto&& srv = m_engine.resource_mgr().create_srv(*tex);
		ImGui::Image(reinterpret_cast<ImTextureID>(srv.ptr), ImVec2(w, h));
	}

	ImGui::End();
}

void Mesh_renderer::draw()
{
	// update camera
	update_camera();

	m_frame_pipeline.m_render_pass_shadow_map->begin_render();
	draw_meshes_shadow_map();
	m_frame_pipeline.m_render_pass_shadow_map->end_render();
	m_frame_pipeline.m_render_pass_main->begin_render();
	draw_meshes();
}

void Mesh_renderer::build_texture()
{
	const auto width		= 256;
	auto&&	   texture_data = TextureDataGenerator::create_checkerboard_texture(width);

	auto&& resource_desc = CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_R8G8B8A8_UNORM, width, width);
	auto&& buffer		 = m_engine.resource_mgr().create_texture(m_texture_name, resource_desc, nullptr, &texture_data);

	m_engine.resource_mgr().register_buffer(m_texture_name, buffer);

	auto&& sampler_desc = D3D12::D3d12x_sampler_desc(D3D12_FILTER_MIN_MAG_MIP_POINT);
	auto&& sampler		= m_engine.resource_mgr().create_sampler(m_point_sampler, sampler_desc);
	m_engine.resource_mgr().register_sampler(m_point_sampler, sampler);
}

void Mesh_renderer::update_camera()
{
	// auto&& cam = m_frame_pipeline.m_camera;
	auto&& t = App::get_duration_app();
	//
	//// XMVECTOR cam_pos    = XMVectorSet(0.0f, 6.0f, -8.0f, 0.0f);
	// XMVECTOR cam_pos    = XMVectorSet(12.0f * sin(t * 0.5f), 8.0f, 12.0f * cos(t * 0.5f), 0.0f);
	// XMVECTOR cam_target = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	// XMVECTOR cam_up     = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	//
	// XMMATRIX view = XMMatrixLookAtLH(cam_pos, cam_target, cam_up);
	//
	// auto&& render_device = m_engine.render_device();
	// auto&& viewport      = render_device.get_window_viewport();
	// auto&& aspect_ratio  = viewport.Width / viewport.Height;
	//
	// XMMATRIX projection = XMMatrixPerspectiveFovLH(0.4f * 3.14f, aspect_ratio, 1.0f, 1000.0f);
	//
	// cam.m_position   = cam_pos;
	// cam.m_view       = view;
	// cam.m_projection = projection;

	// light
	{
		XMVECTOR pos = XMVectorSet(0.0f, 30.0f, 0.0f, 0.0f);
		XMVECTOR dir = XMVectorSet(-1.0f, -1.0f, 0.0f, 0.0f);
		XMVECTOR up	 = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

		XMMATRIX view = XMMatrixLookToLH(pos, dir, up);

		auto&&	 vp_width  = 100.0f;
		auto&&	 vp_height = 100.0f;
		XMMATRIX proj	   = XMMatrixOrthographicLH(vp_width, vp_height, 0.1f, 100.0f);

		m_light.m_view		 = view;
		m_light.m_projection = proj;
	}

	// instances
	// for (int i = 0; i < m_num_instances; ++i) {
	//    float    phase = i * XM_2PI / m_num_instances;
	//    XMVECTOR pos   = XMVectorSet(4.0f * sin(phase), 4.0f + sin(phase + t * 1.5f), 4.0f * cos(phase), 0.0f);
	//    XMStoreFloat4(&m_instance_data[i].pos_world, pos);
	//}
}

void Post_renderer::load_resource()
{
	auto&& device		  = m_engine.render_device();
	auto&& shader_manager = m_engine.shader_mgr();

	{
		string technique_name = "compute_post";

		D3D12::TechniqueInit t;
		t.m_cs = "simple_compute.cs";

		m_engine.shader_mgr().register_technique(technique_name, t);

		// m_compute_post_technique_instance = std::make_shared<D3D12::Technique_instance>(device, shader_manager);
		// m_compute_post_technique_instance->init(technique_name);
	}
}

void Post_renderer::draw()
{
	// post processing
	auto&& render_device = m_engine.render_device();
	auto&& command_list	 = m_engine.render_device().commmand_list();

	auto&& rt_buffer_handle = m_frame_pipeline.m_render_pass_main->render_target_buffer();
	auto&& rt_buffer		= rt_buffer_handle.lock();

	auto&& rt_fmt = m_frame_pipeline.m_render_pass_main->render_target_format();
	auto&& ds_fmt = DXGI_FORMAT_UNKNOWN;

	// render_device.buffer_state_transition(*rt_buffer, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	render_device.buffer_state_transition(*rt_buffer, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

	// auto&& tech_instance = m_compute_post_technique_instance;
	// auto&& tech_handle   = tech_instance->get_technique();
	// auto&& pso           = m_engine.shader_mgr().get_pso(tech_handle, rt_fmt, ds_fmt);
	// if (pso)
	auto&& technique_instance = m_engine.shader_mgr().create_technique_instance("compute_post", rt_fmt, ds_fmt);
	if (technique_instance) {

		technique_instance->set_uav("Texture_uav", rt_buffer);

		// set srv
		auto&& raytrace_buffer = m_frame_pipeline.m_render_pass_raytrace_main->render_target_buffer().lock();
		if (raytrace_buffer) {
			// render_device.buffer_state_transition(*raytrace_buffer, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_GENERIC_READ);
			render_device.buffer_state_transition(*raytrace_buffer, D3D12_RESOURCE_STATE_GENERIC_READ);
			technique_instance->set_srv("Texture_srv", raytrace_buffer);
		}

		command_list()->SetPipelineState(technique_instance->m_pso.Get());
		// set compute root signature
		technique_instance->set_root_signature_parameters(*command_list());

		static uint32_t threadgroup_size = 32;

		uint32_t dispatch_x = (uint32_t)ceilf(rt_buffer->m_d3d_desc.Width / (float)threadgroup_size);
		uint32_t dispatch_y = (uint32_t)ceilf(rt_buffer->m_d3d_desc.Height / (float)threadgroup_size);
		//[skip the postprocess]
		command_list()->Dispatch(dispatch_x, dispatch_y, 1);
	}

	// copy to back buffer
	// render_device.transfer_to_back_buffer(*rt_buffer, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	render_device.transfer_to_back_buffer(*rt_buffer);
}
