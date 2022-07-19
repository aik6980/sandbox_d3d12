

#include "engine.h"

#include "engine_graphic_cpp.h"

//#define USING_IMGUI

void Engine::init(const InitData& initData)
{
    m_render_device = make_unique<D3D12::Device>();
    m_shader_mgr    = make_unique<D3D12::Shader_manager>(*m_render_device);

    m_render_device->LoadPipeline(initData.HWnd);
}

void Engine::load_resource()
{
    // load rendering resources
    m_render_device->begin_load_resources();

    for (auto&& [key, client] : m_engine_client_list) {
        client->load_resource();
    }

    m_render_device->end_load_resources();
}

void Engine::destroy()
{
    m_render_device->wait_for_gpu();
}

void Engine::update()
{
#ifdef USING_IMGUI
    m_render_device->imgui_begin_frame();
    ImGui::ShowDemoWindow();
#endif

    for (auto&& [key, client] : m_engine_client_list) {
        client->update();
    }
}

void Engine::draw()
{
    // Rendering
#ifdef USING_IMGUI
    ImGui::Render();
#endif

    m_render_device->begin_frame();

    // Clear the back buffer and depth buffer.
    // Indicate a state transition on the resource usage.
    auto&& barrier =
        CD3DX12_RESOURCE_BARRIER::Transition(&m_render_device->curr_backbuffer(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
    m_render_device->commmand_list()()->ResourceBarrier(1, &barrier);

    m_render_device->commmand_list()()->ClearRenderTargetView(m_render_device->curr_backbuffer_view(), DirectX::Colors::LightSteelBlue, 0, nullptr);
    m_render_device->commmand_list()()->ClearDepthStencilView(
        m_render_device->curr_backbuffer_depth_stencil_view(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

    // draw through a list of renderer
    for (auto&& [key, client] : m_engine_client_list) {
        client->draw();
    }

#ifdef USING_IMGUI
    m_render_device->imgui_render();
#endif
    // Indicate a state transition on the resource usage.
    barrier = CD3DX12_RESOURCE_BARRIER::Transition(&m_render_device->curr_backbuffer(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
    m_render_device->commmand_list()()->ResourceBarrier(1, &barrier);

    m_render_device->end_frame();

    // Post render
#ifdef USING_IMGUI
    m_render_device->imgui_post_render();
#endif
}

void Engine::register_client(unique_ptr<Engine_client> client)
{
    m_engine_client_list.try_emplace(client->m_name, std::move(client));
}

D3D12::Resource_manager& Engine::resource_mgr()
{
    return m_render_device->resource_manager();
}
