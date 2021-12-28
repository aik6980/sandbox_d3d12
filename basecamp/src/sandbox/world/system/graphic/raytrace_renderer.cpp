#include "raytrace_renderer.h"

#include "common/common.cpp.h"
#include "engine_graphic/engine.h"
#include "engine_graphic/engine_graphic_cpp.h"

void Raytrace_renderer::load_resource()
{
    auto&& device         = m_engine.render_device();
    auto&& shader_manager = m_engine.shader_mgr();
    auto&& resource_mgr   = device.resource_manager();

    {
        string technique_name = "simple_raytracing";
        m_engine.shader_mgr().register_lib_ray_technique(technique_name, "simple_raytracing.ray");

        m_raytrace_technique_instance = std::make_shared<D3D12::Lib_ray_technique_instance>(device, shader_manager);
        m_raytrace_technique_instance->init(technique_name);
    }

    m_render_pass_main = make_unique<Render_pass_main>();
    m_render_pass_main->load_resource();

    // build scene
    // resource_mgr.create_acceleration_structure();
}

void Raytrace_renderer::draw()
{
    m_render_pass_main->begin_render();

    auto&& render_device = m_engine.render_device();
    auto&& resource_mgr  = render_device.resource_manager();
    auto&& command_list  = render_device.commmand_list();

    auto&& main_colour_buffer = m_render_pass_main->render_target_buffer().lock();
    render_device.buffer_state_transition(*main_colour_buffer, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

    auto&& technique = m_engine.shader_mgr().get_lib_ray_technique("simple_raytracing").lock();
    if (technique) {

        auto&& raygen_tbl   = resource_mgr.request_buffer(technique->m_raygen_shader_table_buffer).lock();
        auto&& miss_tbl     = resource_mgr.request_buffer(technique->m_miss_shader_table_buffer).lock();
        auto&& hitgroup_tbl = resource_mgr.request_buffer(technique->m_hitgroup_shader_table_buffer).lock();

        if (raygen_tbl && miss_tbl && hitgroup_tbl) {

            m_raytrace_technique_instance->set_uav("Texture_uav", main_colour_buffer);

            D3D12_DISPATCH_RAYS_DESC dispatch_desc               = {};
            dispatch_desc.HitGroupTable.StartAddress             = hitgroup_tbl->m_buffer->GetGPUVirtualAddress();
            dispatch_desc.HitGroupTable.SizeInBytes              = hitgroup_tbl->m_buffer->GetDesc().Width;
            dispatch_desc.HitGroupTable.StrideInBytes            = dispatch_desc.HitGroupTable.SizeInBytes;
            dispatch_desc.MissShaderTable.StartAddress           = miss_tbl->m_buffer->GetGPUVirtualAddress();
            dispatch_desc.MissShaderTable.SizeInBytes            = miss_tbl->m_buffer->GetDesc().Width;
            dispatch_desc.MissShaderTable.StrideInBytes          = dispatch_desc.MissShaderTable.SizeInBytes;
            dispatch_desc.RayGenerationShaderRecord.StartAddress = raygen_tbl->m_buffer->GetGPUVirtualAddress();
            dispatch_desc.RayGenerationShaderRecord.SizeInBytes  = raygen_tbl->m_buffer->GetDesc().Width;
            dispatch_desc.Width                                  = main_colour_buffer->m_d3d_desc.Width;
            dispatch_desc.Height                                 = main_colour_buffer->m_d3d_desc.Height;
            dispatch_desc.Depth                                  = 1;

            command_list()->SetPipelineState1(technique->m_dxr_state_object.Get());
            command_list()->DispatchRays(&dispatch_desc);
        }
    }

    // copy to back buffer
    auto&& rt_buffer_handle = m_render_pass_main->render_target_buffer();
    auto&& rt_buffer        = rt_buffer_handle.lock();
    render_device.transfer_to_back_buffer(*rt_buffer, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
}
