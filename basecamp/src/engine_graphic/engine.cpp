

#include "engine.h"

#include "engine_graphic_cpp.h"

#define USING_IMGUI

void Engine::init(const InitData& initData)
{
	m_render_device = make_unique<D3D12::Device>();
	m_shader_mgr	= make_unique<D3D12::Shader_manager>(*m_render_device);

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

	// test 2nd imgui window
	ImGui::Begin("Editor Panel");
	ImGui::Text("Text Label 1");

	auto&& window_width	 = ImGui::GetContentRegionAvail().x;
	auto&& window_height = ImGui::GetContentRegionAvail().y;

	auto&& window_pos = ImGui::GetCursorScreenPos();

	const ImU32 white_col = IM_COL32(255, 255, 255, 255);
	ImGui::GetWindowDrawList()->AddCircleFilled(ImVec2(window_pos.x + 100, window_pos.y + 100), 50, white_col);

	// test draw texture
	//auto&& tex = resource_mgr().request_buffer("main_colour_buffer").lock();
	//if (tex) {
	//	auto&& srv = resource_mgr().create_srv(*tex);
	//	ImGui::Image(reinterpret_cast<ImTextureID>(srv.ptr), ImVec2(256, 256));
	//}

	ImGui::Button("Button 1");
	ImGui::End();
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
