#include "render_pass_main.h"

#include "app.h"
#include "engine_graphic/engine_graphic_cpp.h"

void Render_pass_main::begin_render()
{
    auto&& render_device = App::engine().render_device();
    auto&& command_list  = App::engine().render_device().commmand_list();

    command_list()->OMSetRenderTargets(1, &render_device.curr_backbuffer_view(), true, &render_device.curr_backbuffer_depth_stencil_view());
    command_list()->RSSetViewports(1, &render_device.get_window_viewport());
    command_list()->RSSetScissorRects(1, &render_device.get_window_rect());
}

void Render_pass_main::end_render() {}

DXGI_FORMAT Render_pass_main::render_target_format()
{
    return App::engine().render_device().curr_backbuffer().GetDesc().Format;
}

DXGI_FORMAT Render_pass_main::depth_stencil_format()
{
    return App::engine().render_device().get_depth_stencil_format();
}

void Render_pass_shadow_map::load_resource()
{
    auto&& device = App::engine().render_device();

    auto&& desc          = device.get_swap_chain_desc();
    auto&& resource_desc = CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_R32_TYPELESS, desc.BufferDesc.Width, desc.BufferDesc.Height);
    resource_desc.Flags  = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

    auto&& resource_mgr = App::engine().resource_mgr();
    auto&& buffer       = resource_mgr.create_texture(m_depth_buffer_id, resource_desc, &m_depth_buffer_clear_val, nullptr);

    if (resource_mgr.register_buffer(m_depth_buffer_id, buffer)) {
        resource_mgr.create_srv(*buffer, resource_desc);
        resource_mgr.create_dsv(*buffer, resource_desc);
    }
}

void Render_pass_shadow_map::begin_render()
{
    auto&& render_device = App::engine().render_device();
    auto&& command_list  = App::engine().render_device().commmand_list();

    // Specify the buffers we are going to render to.
    auto&& resource_mgr = App::engine().resource_mgr();
    auto&& depth_buffer = resource_mgr.request_buffer(m_depth_buffer_id);

    auto&& cpu_descriptor_handle = render_device.get_dsv_cpu_descriptor_handle(depth_buffer);
    if (get<bool>(cpu_descriptor_handle)) {

        auto&& depth_stencil_view = std::get<CD3DX12_CPU_DESCRIPTOR_HANDLE>(cpu_descriptor_handle);

        // using promoted COMMON
        render_device.buffer_state_transition(*depth_buffer.lock(), D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_DEPTH_WRITE);

        command_list()->ClearDepthStencilView(depth_stencil_view, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);
        command_list()->OMSetRenderTargets(0, nullptr, true, &std::get<CD3DX12_CPU_DESCRIPTOR_HANDLE>(cpu_descriptor_handle));
        command_list()->RSSetViewports(1, &render_device.get_window_viewport());
        command_list()->RSSetScissorRects(1, &render_device.get_window_rect());
    }
}

void Render_pass_shadow_map::end_render()
{
    auto&& resource_mgr = App::engine().resource_mgr();
    auto&& depth_buffer = resource_mgr.request_buffer(m_depth_buffer_id);

    auto&& render_device = App::engine().render_device();
    render_device.buffer_state_transition(*depth_buffer.lock(), D3D12_RESOURCE_STATE_DEPTH_WRITE, D3D12_RESOURCE_STATE_GENERIC_READ);
}

weak_ptr<D3D12::Buffer> Render_pass_shadow_map::depth_stencil_buffer()
{
    auto&& resource_mgr = App::engine().resource_mgr();
    auto&& depth_buffer = resource_mgr.request_buffer(m_depth_buffer_id);

    return depth_buffer;
}
