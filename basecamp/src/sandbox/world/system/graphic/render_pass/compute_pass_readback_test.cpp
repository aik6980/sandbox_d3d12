#include "compute_pass_readback_test.h"

#include "app.h"
#include "engine_graphic/engine_graphic_cpp.h"

void Compute_pass_readback_test::load_resource()
{
    auto&& device = App::engine().render_device();

    auto&& desc             = device.get_swap_chain_desc();
    auto&& resource_desc    = CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_R8G8B8A8_UNORM, desc.BufferDesc.Width, desc.BufferDesc.Height);
    resource_desc.MipLevels = 1;
    resource_desc.Flags     = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET | D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;

    auto&& resource_mgr = App::engine().resource_mgr();

    m_main_colour_buffer_clear_val = CD3DX12_CLEAR_VALUE(DXGI_FORMAT_R8G8B8A8_UNORM, DirectX::Colors::LightSteelBlue);
    auto&& buffer =
        resource_mgr.create_texture(m_main_colour_buffer_id, resource_desc, &m_main_colour_buffer_clear_val, nullptr, D3D12_RESOURCE_STATE_RENDER_TARGET);

    if (resource_mgr.register_buffer(m_main_colour_buffer_id, buffer)) {
        resource_mgr.create_rtv(*buffer, resource_desc);
    }

    vector<uint32_t> resource_data = {120, 45, 90};
    auto&&				  resource_name = "srcbuffer_readback_test";

    D3D12::Buffer_request resource_req;
	resource_req.m_struct_stride	 = sizeof(resource_data.data()[0]);
	resource_req.desc				 = CD3DX12_RESOURCE_DESC::Buffer(resource_data.size() * resource_req.m_struct_stride);
	resource_req.lifetime_persistent = true;
	auto&& buffer					 = resource_mgr.create_buffer(resource_name, resource_req, resource_data.data(), D3D12_RESOURCE_STATE_GENERIC_READ);

    if(buffer)
}

void Compute_pass_readback_test::begin_render()
{
    auto&& render_device = App::engine().render_device();
    auto&& command_list  = App::engine().render_device().commmand_list();

    // Specify the buffers we are going to render to.
    auto&& resource_mgr = App::engine().resource_mgr();
    auto&& rt_buffer    = resource_mgr.request_buffer(m_main_colour_buffer_id);

    auto&& cpu_descriptor_handle = render_device.get_rtv_cpu_descriptor_handle(rt_buffer);
    if (get<bool>(cpu_descriptor_handle)) {

        auto&& view = std::get<CD3DX12_CPU_DESCRIPTOR_HANDLE>(cpu_descriptor_handle);

        // using promoted COMMON
        // render_device.buffer_state_transition(*rt_buffer.lock(), D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_RENDER_TARGET);
        render_device.buffer_state_transition(*rt_buffer.lock(), D3D12_RESOURCE_STATE_RENDER_TARGET);

        command_list()->ClearRenderTargetView(view, m_main_colour_buffer_clear_val.Color, 0, nullptr);
        auto&& dsv = render_device.curr_backbuffer_depth_stencil_view();
        command_list()->OMSetRenderTargets(1, &view, true, &dsv);
        auto&& vp = render_device.get_window_viewport();
        command_list()->RSSetViewports(1, &vp);
        auto&& rect = render_device.get_window_rect();
        command_list()->RSSetScissorRects(1, &rect);
    }
}

void Compute_pass_readback_test::end_render()
{
}

