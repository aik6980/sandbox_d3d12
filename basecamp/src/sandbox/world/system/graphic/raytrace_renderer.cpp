#include "raytrace_renderer.h"

#include "app.h"
#include "common/common_cpp.h"
#include "engine_graphic/engine_graphic_cpp.h"
#include "frame_pipeline.h"
#include "global.h"

void Raytrace_renderer::load_resource()
{
    auto&& device         = m_engine.render_device();
    auto&& shader_manager = m_engine.shader_mgr();
    auto&& resource_mgr   = device.resource_manager();

    {
        m_technique_raytracing_simple_lighting = "raytrace_simplelighting";
        m_engine.shader_mgr().register_lib_ray_technique(m_technique_raytracing_simple_lighting, "raytrace_simplelighting.ray");

        // m_raytrace_technique_instance = std::make_shared<D3D12::Lib_ray_technique_instance>(device, shader_manager);
        // m_raytrace_technique_instance->init(technique_name);
    }

    m_frame_pipeline.m_render_pass_raytrace_main = make_unique<Render_pass_raytrace_main>();
    m_frame_pipeline.m_render_pass_raytrace_main->load_resource();

    // build geometry
    // MeshVertexArray verts;
    // MeshIndexArray  indices;

    // MeshDataGenerator::create_unit_cube(verts, indices);
    // vector<RT_vertex> mesh_verts = MeshDataGenerator::to_rt(verts);

    // m_unit_quad_name   = build_mesh(mesh_verts, indices, "unit_quad_rt", m_engine);
    // auto&& mesh_buffer = m_engine.resource_mgr().request_mesh_buffer(m_unit_quad_name).lock();
    //  build scene
    //  resource_mgr.create_acceleration_structure("rtaccel_structure_buffer", *mesh_buffer, true);
    //  m_rtaccel_structure_buffer_handle = "rtaccel_structure_buffer_tlas";

    {
        auto&& mesh_name  = "grid_mesh_rt";
        auto&& mesh_data  = MeshDataGenerator::create_grid(25.0, 25.0, 10, 10);
        auto&& mesh_verts = MeshDataGenerator::to_rt(get<MeshVertexArray>(mesh_data));
        m_grid_mesh       = build_mesh(mesh_verts, get<MeshIndexArray>(mesh_data), mesh_name, m_engine);

        m_mesh_list[mesh_name]               = make_unique<Mesh_data_raw>();
        m_mesh_list[mesh_name]->vertices_raw = make_unique<MeshVertexArray>(get<MeshVertexArray>(mesh_data));
        m_mesh_list[mesh_name]->indices_raw  = make_unique<MeshIndexArray>(get<MeshIndexArray>(mesh_data));
        m_mesh_list[mesh_name]->vertices_fat = MeshDataGenerator::to_fat(get<MeshVertexArray>(mesh_data));
    }

    {
        auto&& mesh_name  = "cube_mesh_rt";
        auto&& mesh_data  = MeshDataGenerator::create_unit_cube();
        auto&& mesh_verts = MeshDataGenerator::to_rt(get<MeshVertexArray>(mesh_data));
        m_unit_cube_name  = build_mesh(mesh_verts, get<MeshIndexArray>(mesh_data), "cube_mesh_rt", m_engine);

        m_mesh_list[mesh_name]               = make_unique<Mesh_data_raw>();
        m_mesh_list[mesh_name]->vertices_raw = make_unique<MeshVertexArray>(get<MeshVertexArray>(mesh_data));
        m_mesh_list[mesh_name]->indices_raw  = make_unique<MeshIndexArray>(get<MeshIndexArray>(mesh_data));
        m_mesh_list[mesh_name]->vertices_fat = MeshDataGenerator::to_fat(get<MeshVertexArray>(mesh_data));
    }

    m_scene_data = make_unique<D3D12::Scene_data>();
}

void Raytrace_renderer::update()
{
    // update scene
    m_scene_data->m_instance_transforms.clear();

    auto&& t = App::get_duration_app();
    // add instances
    uint32_t num_instances = 5;
    for (uint32_t i = 0; i < num_instances; ++i) {
        float    phase = i * XM_2PI / num_instances;
        XMVECTOR pos   = XMVectorSet(4.0f * sin(phase), 4.0f + sin(phase + t * 1.5f), 4.0f * cos(phase), 0.0f);

        auto&& trans_mat = XMMatrixTranslationFromVector(pos);
        m_scene_data->add_instance(m_unit_cube_name, trans_mat);
    }

    auto&& trans_mat = XMMatrixIdentity();
    m_scene_data->add_instance(m_grid_mesh, trans_mat);

    auto&& cube_trans_mat = XMMatrixTranslation(0.0f, 1.0f, 0.0f);
    m_scene_data->add_instance(m_unit_cube_name, cube_trans_mat);
}

void Raytrace_renderer::draw()
{
    auto&& render_device = m_engine.render_device();
    auto&& resource_mgr  = render_device.resource_manager();
    auto&& command_list  = render_device.commmand_list();

    // re-build acceleration struture
    auto&& mesh_buffer    = m_engine.resource_mgr().request_mesh_buffer(m_grid_mesh).lock();
    auto&& rtaccel_buffer = resource_mgr.create_acceleration_structure("rtaccel_structure_buffer", *m_scene_data, false);
    // auto&& rtaccel_buffer = resource_mgr.create_acceleration_structure("rtaccel_structure_buffer", *mesh_buffer, false);
    //  m_rtaccel_structure_buffer_handle = "rtaccel_structure_buffer_tlas";

    auto&& scene_data_buffers = build_scene_attrib_buffer();

    auto&& main_colour_buffer = m_frame_pipeline.m_render_pass_raytrace_main->render_target_buffer().lock();
    // render_device.buffer_state_transition(*main_colour_buffer, D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
    render_device.buffer_state_transition(*main_colour_buffer, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

    auto&& technique          = m_engine.shader_mgr().get_lib_ray_technique(m_technique_raytracing_simple_lighting).lock();
    auto&& technique_instance = m_engine.shader_mgr().create_lib_ray_technique_instance(m_technique_raytracing_simple_lighting);
    if (technique) {

        auto&& raygen_tbl   = resource_mgr.request_buffer(technique->m_raygen_shader_table_buffer).lock();
        auto&& miss_tbl     = resource_mgr.request_buffer(technique->m_miss_shader_table_buffer).lock();
        auto&& hitgroup_tbl = resource_mgr.request_buffer(technique->m_hitgroup_shader_table_buffer).lock();

        // auto&& rtaccel_buffer = resource_mgr.request_buffer(m_rtaccel_structure_buffer_handle).lock();

        if (raygen_tbl && miss_tbl && hitgroup_tbl) {

            auto&& cam = m_frame_pipeline.m_camera;
            technique_instance->set_cbv("Camera_cb", "Camera_projection_to_world", cam.projection_to_world());
            technique_instance->set_cbv("Camera_cb", "Camera_world_pos", cam.position());

            technique_instance->set_uav("Output_uav", main_colour_buffer);
            technique_instance->set_srv("Scene_srv", rtaccel_buffer);

            auto&& viewport = XMFLOAT4(-1.0f, -1.0f, 1.0f, 1.0f);
            auto&& stencil  = XMFLOAT4(-1.0f * 0.9f, -1.0f * 0.9f, 1.0f * 0.9f, 1.0f * 0.9f);
            technique_instance->set_cbv("Raygen_cb", "Main_vp", viewport);
            technique_instance->set_cbv("Raygen_cb", "Stencil_vp", stencil);

            // scene attrib
            technique_instance->set_srv("Instance_data_srv", scene_data_buffers.instance_data_buffer);
            technique_instance->set_srv("Mesh_data_srv", scene_data_buffers.mesh_data_buffer);
            technique_instance->set_srv("Vertices_srv", scene_data_buffers.vertex_buffer);
            technique_instance->set_srv("Indices_srv", scene_data_buffers.index_buffer);

            D3D12_DISPATCH_RAYS_DESC dispatch_desc               = {};
            dispatch_desc.HitGroupTable.StartAddress             = hitgroup_tbl->m_buffer->GetGPUVirtualAddress();
            dispatch_desc.HitGroupTable.SizeInBytes              = hitgroup_tbl->m_buffer->GetDesc().Width;
            dispatch_desc.HitGroupTable.StrideInBytes            = dispatch_desc.HitGroupTable.SizeInBytes;
            dispatch_desc.MissShaderTable.StartAddress           = miss_tbl->m_buffer->GetGPUVirtualAddress();
            dispatch_desc.MissShaderTable.SizeInBytes            = miss_tbl->m_buffer->GetDesc().Width;
            dispatch_desc.MissShaderTable.StrideInBytes          = dispatch_desc.MissShaderTable.SizeInBytes;
            dispatch_desc.RayGenerationShaderRecord.StartAddress = raygen_tbl->m_buffer->GetGPUVirtualAddress();
            dispatch_desc.RayGenerationShaderRecord.SizeInBytes  = raygen_tbl->m_buffer->GetDesc().Width;
            dispatch_desc.Width                                  = (uint32_t)main_colour_buffer->m_d3d_desc.Width;
            dispatch_desc.Height                                 = (uint32_t)main_colour_buffer->m_d3d_desc.Height;
            dispatch_desc.Depth                                  = 1;

            command_list()->SetPipelineState1(technique->m_dxr_state_object.Get());

            technique_instance->set_root_signature_parameters(*command_list());

            command_list()->DispatchRays(&dispatch_desc);
        }
    }

    // copy to back buffer
    // auto&& rt_buffer_handle = m_render_pass_raytrace_main->render_target_buffer();
    // auto&& rt_buffer        = rt_buffer_handle.lock();
    // render_device.transfer_to_back_buffer(*rt_buffer, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
}

Scene_data_buffer Raytrace_renderer::build_scene_attrib_buffer()
{
    // build mesh data buffer
    vector<Fat_vertex> vertices;
    vector<uint32_t>   indices;
    vector<Mesh_data>  mesh_data;
    uint32_t           total_vertices = 0;
    uint32_t           total_indices  = 0;

    mesh_data.reserve(m_scene_data->m_instance_transforms.size());
    vertices.reserve(256);
    indices.reserve(256);

    for (auto&& obj : m_scene_data->m_instance_transforms) {
        auto&& found = m_mesh_list.find(obj.first);
        if (found == m_mesh_list.end()) {
            throw;
        }

        Mesh_data mesh;
        mesh.m_num_vertices    = found->second->vertices_fat.size();
        mesh.m_num_indices     = found->second->indices_raw->m_indices32.size();
        mesh.m_offset_vertices = total_vertices;
        mesh.m_offset_indices  = total_indices;

        total_vertices += mesh.m_num_vertices;
        total_indices += mesh.m_num_indices;

        mesh_data.emplace_back(mesh);

        // concat to the attrib buffer
        vertices.insert(vertices.end(), found->second->vertices_fat.begin(), found->second->vertices_fat.end());
        indices.insert(indices.end(), found->second->indices_raw->m_indices32.begin(), found->second->indices_raw->m_indices32.end());
    }

    // build instance buffer
    vector<Instance_data> instance_data;
    instance_data.reserve(m_scene_data->num_instances());
    uint32_t mesh_id = 0;
    for (auto&& obj : m_scene_data->m_instance_transforms) {
        for (auto&& obj_instance : obj.second) {
            Instance_data instance;
            instance.m_mesh_id = mesh_id;

            instance_data.emplace_back(instance);
        }

        mesh_id++;
    }

    // build buffer
    auto&& render_device = m_engine.render_device();
    auto&& resource_mgr  = render_device.resource_manager();

    Scene_data_buffer output;
    {
        auto&& resource_data = instance_data;
        auto&& resource_name = "scene_data_instance_buffer";

        D3D12::Buffer_request resource_req;
        resource_req.m_struct_stride     = sizeof(resource_data.data()[0]);
        resource_req.desc                = CD3DX12_RESOURCE_DESC::Buffer(resource_data.size() * resource_req.m_struct_stride);
        resource_req.lifetime_persistent = false;
        auto&& buffer                    = resource_mgr.create_buffer(resource_name, resource_req, resource_data.data(), D3D12_RESOURCE_STATE_GENERIC_READ);

        output.instance_data_buffer = buffer;
    }

    {
        auto&& resource_data = mesh_data;
        auto&& resource_name = "scene_data_mesh_data_buffer";

        D3D12::Buffer_request resource_req;
        resource_req.m_struct_stride     = sizeof(resource_data.data()[0]);
        resource_req.desc                = CD3DX12_RESOURCE_DESC::Buffer(resource_data.size() * resource_req.m_struct_stride);
        resource_req.lifetime_persistent = false;
        auto&& buffer                    = resource_mgr.create_buffer(resource_name, resource_req, resource_data.data(), D3D12_RESOURCE_STATE_GENERIC_READ);

        output.mesh_data_buffer = buffer;
    }

    {
        auto&& resource_data = vertices;
        auto&& resource_name = "scene_data_vertex_buffer";

        D3D12::Buffer_request resource_req;
        resource_req.m_struct_stride     = sizeof(resource_data.data()[0]);
        resource_req.desc                = CD3DX12_RESOURCE_DESC::Buffer(resource_data.size() * resource_req.m_struct_stride);
        resource_req.lifetime_persistent = false;
        auto&& buffer                    = resource_mgr.create_buffer(resource_name, resource_req, resource_data.data(), D3D12_RESOURCE_STATE_GENERIC_READ);

        output.vertex_buffer = buffer;
    }

    {
        auto&& resource_data = indices;
        auto&& resource_name = "scene_data_index_buffer";

        D3D12::Buffer_request resource_req;
        resource_req.m_struct_stride     = sizeof(resource_data.data()[0]);
        resource_req.desc                = CD3DX12_RESOURCE_DESC::Buffer(resource_data.size() * resource_req.m_struct_stride);
        resource_req.lifetime_persistent = false;
        auto&& buffer                    = resource_mgr.create_buffer(resource_name, resource_req, resource_data.data(), D3D12_RESOURCE_STATE_GENERIC_READ);

        output.index_buffer = buffer;
    }

    return output;
}
