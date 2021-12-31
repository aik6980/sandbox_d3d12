

#include "mesh_renderer.h"

#include "app.h"
#include "common/common.cpp.h"
#include "engine_graphic/engine.h"
#include "engine_graphic/engine_graphic_cpp.h"
#include "frame_pipeline.h"
#include "global.h"
#include "world/component/camera.h"

void Mesh_renderer::load_resource()
{
    auto&& device         = m_engine.render_device();
    auto&& shader_manager = m_engine.shader_mgr();

    {
        string               technique_name = "default";
        D3D12::TechniqueInit t;
        t.m_vs = "default.vs";
        t.m_ps = "default.ps";

        m_engine.shader_mgr().register_technique(technique_name, t);

        m_render_technique_instance = std::make_shared<D3D12::TechniqueInstance>(device, shader_manager);
        m_render_technique_instance->init(technique_name);

        m_render_technique_instance1 = std::make_shared<D3D12::TechniqueInstance>(device, shader_manager);
        m_render_technique_instance1->init(technique_name);
    }

    {
        string               technique_name = "mesh";
        D3D12::TechniqueInit t;
        t.m_vs = "mesh.vs";
        t.m_ps = "mesh.ps";

        m_engine.shader_mgr().register_technique(technique_name, t);

        m_render_technique_mesh_instance = std::make_shared<D3D12::TechniqueInstance>(device, shader_manager);
        m_render_technique_mesh_instance->init(technique_name);

        m_render_technique_grid_mesh_instance = std::make_shared<D3D12::TechniqueInstance>(device, shader_manager);
        m_render_technique_grid_mesh_instance->init(technique_name);
    }

    {
        string               technique_name = "mesh_shadow_map";
        D3D12::TechniqueInit t;
        t.m_vs = "mesh.vs";
        t.m_ps = "";

        m_engine.shader_mgr().register_technique(technique_name, t);

        m_shadow_map_technique_instance = std::make_shared<D3D12::TechniqueInstance>(device, shader_manager);
        m_shadow_map_technique_instance->init(technique_name);
    }

    {
        string               technique_name = "mesh_instancing";
        D3D12::TechniqueInit t;
        t.m_vs = "mesh_instancing.vs";
        t.m_ps = "mesh.ps";

        m_engine.shader_mgr().register_technique(technique_name, t);

        m_mesh_instancing_technique_instance = std::make_shared<D3D12::TechniqueInstance>(device, shader_manager);
        m_mesh_instancing_technique_instance->init(technique_name);
    }

    // build a mesh
    build_quad_mesh();
    build_cube_mesh();

    auto&& mesh_data  = MeshDataGenerator::create_grid(25.0, 25.0, 10, 10);
    auto&& mesh_verts = MeshDataGenerator::to_p1c1(get<MeshVertexArray>(mesh_data));
    m_grid_mesh       = build_mesh(mesh_verts, get<MeshIndexArray>(mesh_data), "grid_mesh", m_engine);

    build_texture();

    // create an instance buffer
    m_instance_data.resize(m_num_instances);
    m_engine.resource_mgr().create_instance_buffer(m_instance_data_buffer_name, sizeof(Instance_data) * m_num_instances);
}

float key_i = 1.0f;

void Mesh_renderer::draw_meshes_shadow_map()
{
    auto&& camera = m_light;

    auto&& rt_fmt = m_frame_pipeline.m_render_pass_shadow_map->render_target_format();
    auto&& ds_fmt = m_frame_pipeline.m_render_pass_shadow_map->depth_stencil_format();

    auto&& command_list = m_engine.render_device().commmand_list();

    auto&& mesh_handle      = m_engine.resource_mgr().request_mesh_buffer(m_unit_cube_name);
    auto&& mesh_buffer      = mesh_handle.lock();
    auto&& mesh_tech_handle = m_shadow_map_technique_instance->get_technique();
    auto&& pso              = m_engine.shader_mgr().get_pso(mesh_tech_handle, rt_fmt, ds_fmt);
    if (pso && mesh_buffer) {

        m_shadow_map_technique_instance->set_cbv("Camera_cb", "View", &camera.view(), sizeof(camera.view()));
        m_shadow_map_technique_instance->set_cbv("Camera_cb", "Projection", &camera.projection(), sizeof(camera.projection()));

        XMMATRIX world = XMMatrixTranslation(0.0f, 1.0f, 0.0f);
        m_shadow_map_technique_instance->set_cbv("Object_cb", "World", &world, sizeof(world));

        auto&& tex = m_engine.resource_mgr().request_buffer(m_texture_name);
        m_shadow_map_technique_instance->set_srv("Diffuse_srv", tex);

        command_list()->SetPipelineState(pso.Get());
        m_shadow_map_technique_instance->set_root_signature_parameters(*command_list());

        command_list()->IASetVertexBuffers(0, 1, &mesh_buffer->vertex_buffer_view());
        command_list()->IASetIndexBuffer(&mesh_buffer->index_buffer_view());
        command_list()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        auto&& index_count = mesh_buffer->m_mesh_location.index_count;
        command_list()->DrawIndexedInstanced(index_count, 1, 0, 0, 0);
    }
}

void Mesh_renderer::draw_meshes()
{
    auto&& rt_fmt = m_frame_pipeline.m_render_pass_main->render_target_format();
    auto&& ds_fmt = m_frame_pipeline.m_render_pass_main->depth_stencil_format();

    m_render_technique_instance->set_cbv("key", "key_i", &key_i, sizeof(key_i));

    float vec[2] = {0.25f * sinf(0.001f), 0.0f};
    m_render_technique_instance->set_cbv("cb_vs", "offset", vec, sizeof(vec));

    float scale[2] = {1.0, 1.0};
    m_render_technique_instance->set_cbv("cb_vs", "scale", scale, sizeof(scale));
    scale[0] = scale[1] = 0.25f;
    m_render_technique_instance1->set_cbv("cb_vs", "scale", scale, sizeof(scale));

    float colour[4] = {0.9f, 0.1f, 0.9f, 1.0f};
    m_render_technique_instance->set_cbv("cb_ps", "colour", colour, sizeof(colour));

    float offsety = 0.25f * sinf(0.001f);
    m_render_technique_instance1->set_cbv("cb_vs", "offsety", &offsety, sizeof(offsety));

    // begine gpu work this frame
    auto&& render_device = m_engine.render_device();
    auto&& command_list  = m_engine.render_device().commmand_list();

    auto&& tech_handle = m_render_technique_instance->get_technique();
    if (auto tech = tech_handle.lock()) {
        auto&& pso = m_engine.shader_mgr().get_pso(tech_handle, rt_fmt, ds_fmt);
        command_list()->SetPipelineState(pso.Get());
        m_render_technique_instance->set_root_signature_parameters(*command_list());

        command_list()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        // command_list()->DrawInstanced(3, 1, 0, 0);
    }

    auto&& tech_handle1 = m_render_technique_instance1->get_technique();
    if (auto tech = tech_handle1.lock()) {
        auto&& pso = m_engine.shader_mgr().get_pso(tech_handle1, rt_fmt, ds_fmt);
        command_list()->SetPipelineState(pso.Get());
        m_render_technique_instance1->set_root_signature_parameters(*command_list());

        command_list()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        // command_list()->DrawInstanced(3, 1, 0, 0);
    }

    auto&& mesh_handle      = m_engine.resource_mgr().request_mesh_buffer(m_unit_cube_name);
    auto&& mesh_buffer      = mesh_handle.lock();
    auto&& mesh_tech_handle = m_render_technique_mesh_instance->get_technique();
    auto&& pso              = m_engine.shader_mgr().get_pso(mesh_tech_handle, rt_fmt, ds_fmt);
    if (pso && mesh_buffer) {

        m_render_technique_mesh_instance->set_cbv("Camera_cb", "View", &m_camera.view(), sizeof(m_camera.view()));
        m_render_technique_mesh_instance->set_cbv("Camera_cb", "Projection", &m_camera.projection(), sizeof(m_camera.projection()));

        m_render_technique_grid_mesh_instance->set_cbv("Light_cb", "Receive_shadow", 0);

        XMMATRIX world = XMMatrixTranslation(0.0f, 1.0f, 0.0f);
        m_render_technique_mesh_instance->set_cbv("Object_cb", "World", &world, sizeof(world));

        auto&& tex = m_engine.resource_mgr().request_buffer(m_texture_name);
        m_render_technique_mesh_instance->set_srv("Diffuse_srv", tex);

        command_list()->SetPipelineState(pso.Get());
        m_render_technique_mesh_instance->set_root_signature_parameters(*command_list());

        command_list()->IASetVertexBuffers(0, 1, &mesh_buffer->vertex_buffer_view());
        command_list()->IASetIndexBuffer(&mesh_buffer->index_buffer_view());
        command_list()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        auto&& index_count = mesh_buffer->m_mesh_location.index_count;
        command_list()->DrawIndexedInstanced(index_count, 1, 0, 0, 0);
    }

    auto&& mesh_instancing_tech_handle = m_mesh_instancing_technique_instance->get_technique();
    auto&& mesh_instancing_pso         = m_engine.shader_mgr().get_pso(mesh_instancing_tech_handle, rt_fmt, ds_fmt);
    if (mesh_instancing_pso && mesh_buffer) {

        m_mesh_instancing_technique_instance->set_cbv("Camera_cb", "View", &m_camera.view(), sizeof(m_camera.view()));
        m_mesh_instancing_technique_instance->set_cbv("Camera_cb", "Projection", &m_camera.projection(), sizeof(m_camera.projection()));

        m_mesh_instancing_technique_instance->set_cbv("Light_cb", "Receive_shadow", 0);

        auto&& tex = m_engine.resource_mgr().request_buffer(m_texture_name);
        m_mesh_instancing_technique_instance->set_srv("Diffuse_srv", tex);

        // update instance data
        m_engine.resource_mgr().update_dynamic_buffer(
            m_instance_data_buffer_name, m_instance_data.data(), sizeof(Instance_data) * (uint32_t)m_instance_data.size());

        command_list()->SetPipelineState(mesh_instancing_pso.Get());
        m_mesh_instancing_technique_instance->set_root_signature_parameters(*command_list());

        std::array<D3D12_VERTEX_BUFFER_VIEW, 2> vb_views = {
            mesh_buffer->vertex_buffer_view(), m_engine.resource_mgr().request_instance_buffer_view(m_instance_data_buffer_name, sizeof(Instance_data))};

        command_list()->IASetVertexBuffers(0, (uint32_t)vb_views.size(), vb_views.data());
        command_list()->IASetIndexBuffer(&mesh_buffer->index_buffer_view());
        command_list()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        auto&& index_count = mesh_buffer->m_mesh_location.index_count;
        command_list()->DrawIndexedInstanced(index_count, m_instance_data.size(), 0, 0, 0);
    }

    auto&& grid_mesh_handle      = m_engine.resource_mgr().request_mesh_buffer(m_grid_mesh);
    auto&& grid_mesh_buffer      = grid_mesh_handle.lock();
    auto&& grid_mesh_tech_handle = m_render_technique_grid_mesh_instance->get_technique();
    auto&& pso1                  = m_engine.shader_mgr().get_pso(grid_mesh_tech_handle, rt_fmt, ds_fmt);
    if (grid_mesh_buffer && pso1) {

        m_render_technique_grid_mesh_instance->set_cbv("Camera_cb", "View", &m_camera.view(), sizeof(m_camera.view()));
        m_render_technique_grid_mesh_instance->set_cbv("Camera_cb", "Projection", &m_camera.projection(), sizeof(m_camera.projection()));

        m_render_technique_grid_mesh_instance->set_cbv("Light_cb", "Receive_shadow", 1);
        m_render_technique_grid_mesh_instance->set_cbv("Light_cb", "Light_view", &m_light.view(), sizeof(m_light.view()));
        m_render_technique_grid_mesh_instance->set_cbv("Light_cb", "Light_projection", &m_light.projection(), sizeof(m_light.projection()));
        auto&& shadow_map_tex = m_frame_pipeline.m_render_pass_shadow_map->depth_stencil_buffer();
        m_render_technique_grid_mesh_instance->set_srv("Shadow_map_srv", shadow_map_tex);

        auto&& sampler = m_engine.resource_mgr().request_sampler(m_point_sampler);
        m_render_technique_grid_mesh_instance->set_sampler("Point_sampler", sampler);

        XMMATRIX world = XMMatrixTranslation(0.0f, 0.0f, 0.0f);
        m_render_technique_grid_mesh_instance->set_cbv("Object_cb", "World", &world, sizeof(world));

        auto&& tex = m_engine.resource_mgr().request_buffer(m_texture_name);
        m_render_technique_grid_mesh_instance->set_srv("Diffuse_srv", tex);

        command_list()->SetPipelineState(pso1.Get());
        m_render_technique_grid_mesh_instance->set_root_signature_parameters(*command_list());

        command_list()->IASetVertexBuffers(0, 1, &grid_mesh_buffer->vertex_buffer_view());
        command_list()->IASetIndexBuffer(&grid_mesh_buffer->index_buffer_view());
        command_list()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        auto&& index_count = grid_mesh_buffer->m_mesh_location.index_count;
        command_list()->DrawIndexedInstanced(index_count, 1, 0, 0, 0);
    }
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

void Mesh_renderer::build_quad_mesh()
{
    MeshVertexArray verts;
    MeshIndexArray  indices;

    MeshDataGenerator::create_unit_quad(verts, indices);
    vector<P1InputLayout> mesh_verts = MeshDataGenerator::to_p1(verts);

    m_unit_quad_name = build_mesh(mesh_verts, indices, "unit_quad", m_engine);
}

void Mesh_renderer::build_cube_mesh()
{
    MeshVertexArray verts;
    MeshIndexArray  indices;

    MeshDataGenerator::create_unit_cube(verts, indices);
    vector<P1C1InputLayout> mesh_verts = MeshDataGenerator::to_p1c1(verts);

    m_unit_cube_name = build_mesh(mesh_verts, indices, "unit_cube", m_engine);
}

void Mesh_renderer::build_texture()
{
    const auto width        = 256;
    auto&&     texture_data = TextureDataGenerator::create_checkerboard_texture(width);

    auto&& resource_desc = CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_R8G8B8A8_UNORM, width, width);
    auto&& buffer        = m_engine.resource_mgr().create_texture(m_texture_name, resource_desc, nullptr, &texture_data);
    m_engine.resource_mgr().create_srv(*buffer, resource_desc);

    m_engine.resource_mgr().register_buffer(m_texture_name, buffer);

    auto&& sampler_desc = D3D12::D3d12x_sampler_desc(D3D12_FILTER_MIN_MAG_MIP_POINT);
    auto&& sampler      = m_engine.resource_mgr().create_sampler(m_point_sampler, sampler_desc);
    m_engine.resource_mgr().register_sampler(m_point_sampler, sampler);
}

void Mesh_renderer::update_camera()
{
    auto&& t = App::get_duration_app();

    // XMVECTOR cam_pos    = XMVectorSet(0.0f, 6.0f, -8.0f, 0.0f);
    XMVECTOR cam_pos    = XMVectorSet(12.0f * sin(t * 0.5f), 8.0f, 12.0f * cos(t * 0.5f), 0.0f);
    XMVECTOR cam_target = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
    XMVECTOR cam_up     = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

    XMMATRIX view = XMMatrixLookAtLH(cam_pos, cam_target, cam_up);

    auto&& render_device = m_engine.render_device();
    auto&& viewport      = render_device.get_window_viewport();
    auto&& aspect_ratio  = viewport.Width / viewport.Height;

    XMMATRIX projection = XMMatrixPerspectiveFovLH(0.4f * 3.14f, aspect_ratio, 1.0f, 1000.0f);

    m_camera.m_view       = view;
    m_camera.m_projection = projection;

    // light
    {
        XMVECTOR pos = XMVectorSet(0.0f, 30.0f, 0.0f, 0.0f);
        XMVECTOR dir = XMVectorSet(-1.0f, -1.0f, 0.0f, 0.0f);
        XMVECTOR up  = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

        XMMATRIX view = XMMatrixLookToLH(pos, dir, up);

        auto&&   vp_width  = 100.0f;
        auto&&   vp_height = 100.0f;
        XMMATRIX proj      = XMMatrixOrthographicLH(vp_width, vp_height, 0.1f, 100.0f);

        m_light.m_view       = view;
        m_light.m_projection = proj;
    }

    // instances
    for (int i = 0; i < m_num_instances; ++i) {
        float    phase = i * XM_2PI / m_num_instances;
        XMVECTOR pos   = XMVectorSet(4.0f * sin(phase), 4.0f + sin(phase + t * 1.5f), 4.0f * cos(phase), 0.0f);
        XMStoreFloat4(&m_instance_data[i].pos_world, pos);
    }
}

void Post_renderer::load_resource()
{
    auto&& device         = m_engine.render_device();
    auto&& shader_manager = m_engine.shader_mgr();

    {
        string technique_name = "compute_post";

        D3D12::TechniqueInit t;
        t.m_cs = "simple_compute.cs";

        m_engine.shader_mgr().register_technique(technique_name, t);

        m_compute_post_technique_instance = std::make_shared<D3D12::TechniqueInstance>(device, shader_manager);
        m_compute_post_technique_instance->init(technique_name);
    }
}

void Post_renderer::draw()
{
    // post processing
    auto&& render_device = m_engine.render_device();
    auto&& command_list  = m_engine.render_device().commmand_list();

    auto&& rt_buffer_handle = m_frame_pipeline.m_render_pass_main->render_target_buffer();
    auto&& rt_buffer        = rt_buffer_handle.lock();

    auto&& rt_fmt = m_frame_pipeline.m_render_pass_main->render_target_format();
    auto&& ds_fmt = DXGI_FORMAT_UNKNOWN;

    render_device.buffer_state_transition(*rt_buffer, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

    auto&& tech_instance = m_compute_post_technique_instance;
    auto&& tech_handle   = tech_instance->get_technique();
    auto&& pso           = m_engine.shader_mgr().get_pso(tech_handle, rt_fmt, ds_fmt);
    if (pso) {

        tech_instance->set_uav("Texture_uav", rt_buffer);

        // set srv
        auto&& raytrace_buffer = m_frame_pipeline.m_render_pass_raytrace_main->render_target_buffer().lock();
        if (raytrace_buffer) {
            render_device.buffer_state_transition(*raytrace_buffer, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_GENERIC_READ);
            tech_instance->set_srv("Texture_srv", raytrace_buffer);
        }

        command_list()->SetPipelineState(pso.Get());
        // set compute root signature
        tech_instance->set_root_signature_parameters(*command_list());

        static uint32_t threadgroup_size = 32;

        uint32_t dispatch_x = ceilf(rt_buffer->m_d3d_desc.Width / (float)threadgroup_size);
        uint32_t dispatch_y = ceilf(rt_buffer->m_d3d_desc.Height / (float)threadgroup_size);
        command_list()->Dispatch(dispatch_x, dispatch_y, 1);
    }

    // copy to back buffer
    render_device.transfer_to_back_buffer(*rt_buffer, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
}
