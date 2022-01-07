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
        string technique_name = "raytrace_simplelighting";
        m_engine.shader_mgr().register_lib_ray_technique(technique_name, "raytrace_simplelighting.ray");

        m_raytrace_technique_instance = std::make_shared<D3D12::Lib_ray_technique_instance>(device, shader_manager);
        m_raytrace_technique_instance->init(technique_name);
    }

    m_frame_pipeline.m_render_pass_raytrace_main = make_unique<Render_pass_raytrace_main>();
    m_frame_pipeline.m_render_pass_raytrace_main->load_resource();

    // build geometry
    // MeshVertexArray verts;
    // MeshIndexArray  indices;

    // MeshDataGenerator::create_unit_cube(verts, indices);
    // vector<RtInputLayout> mesh_verts = MeshDataGenerator::to_rt(verts);

    // m_unit_quad_name   = build_mesh(mesh_verts, indices, "unit_quad_rt", m_engine);
    // auto&& mesh_buffer = m_engine.resource_mgr().request_mesh_buffer(m_unit_quad_name).lock();
    //  build scene
    //  resource_mgr.create_acceleration_structure("rtaccel_structure_buffer", *mesh_buffer, true);
    //  m_rtaccel_structure_buffer_handle = "rtaccel_structure_buffer_tlas";

    auto&& mesh_data  = MeshDataGenerator::create_grid(25.0, 25.0, 10, 10);
    auto&& mesh_verts = MeshDataGenerator::to_rt(get<MeshVertexArray>(mesh_data));
    m_grid_mesh       = build_mesh(mesh_verts, get<MeshIndexArray>(mesh_data), "grid_mesh_rt", m_engine);

    auto&& cube_mesh_data  = MeshDataGenerator::create_unit_cube();
    auto&& cube_mesh_verts = MeshDataGenerator::to_rt(get<MeshVertexArray>(cube_mesh_data));
    m_unit_cube_name       = build_mesh(cube_mesh_verts, get<MeshIndexArray>(cube_mesh_data), "cube_mesh_rt", m_engine);

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

    auto&& main_colour_buffer = m_frame_pipeline.m_render_pass_raytrace_main->render_target_buffer().lock();
    // render_device.buffer_state_transition(*main_colour_buffer, D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
    render_device.buffer_state_transition(*main_colour_buffer, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

    auto&& technique = m_engine.shader_mgr().get_lib_ray_technique("raytrace_simplelighting").lock();
    if (technique) {

        auto&& raygen_tbl   = resource_mgr.request_buffer(technique->m_raygen_shader_table_buffer).lock();
        auto&& miss_tbl     = resource_mgr.request_buffer(technique->m_miss_shader_table_buffer).lock();
        auto&& hitgroup_tbl = resource_mgr.request_buffer(technique->m_hitgroup_shader_table_buffer).lock();

        // auto&& rtaccel_buffer = resource_mgr.request_buffer(m_rtaccel_structure_buffer_handle).lock();

        if (raygen_tbl && miss_tbl && hitgroup_tbl) {

            auto&& cam = m_frame_pipeline.m_camera;
            m_raytrace_technique_instance->set_cbv("Camera_cb", "Camera_projection_to_world", cam.projection_to_world());
            m_raytrace_technique_instance->set_cbv("Camera_cb", "Camera_world_pos", cam.position());

            m_raytrace_technique_instance->set_uav("Output_uav", main_colour_buffer);
            m_raytrace_technique_instance->set_srv("Scene_srv", rtaccel_buffer);

            auto&& viewport = XMFLOAT4(-1.0f, -1.0f, 1.0f, 1.0f);
            auto&& stencil  = XMFLOAT4(-1.0f * 0.9f, -1.0f * 0.9f, 1.0f * 0.9f, 1.0f * 0.9f);
            m_raytrace_technique_instance->set_cbv("Raygen_cb", "Main_vp", viewport);
            m_raytrace_technique_instance->set_cbv("Raygen_cb", "Stencil_vp", stencil);

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

            m_raytrace_technique_instance->set_root_signature_parameters(*command_list());

            command_list()->DispatchRays(&dispatch_desc);
        }
    }

    // copy to back buffer
    // auto&& rt_buffer_handle = m_render_pass_raytrace_main->render_target_buffer();
    // auto&& rt_buffer        = rt_buffer_handle.lock();
    // render_device.transfer_to_back_buffer(*rt_buffer, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
}
