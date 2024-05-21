#include "device.h"

#include "frame_resource.h"

#include "common/debug/debug_output.h"
#include "common/debug/debug_util.h"

#include "imgui.h"
#include "imgui/imgui_impl_dx12.h"
#include "imgui/imgui_impl_win32.h"

#include "resource_manager.h"

namespace D3D12 {
	static const D3D_FEATURE_LEVEL REQUESTED_FEATURE_LEVEL = D3D_FEATURE_LEVEL_12_1;

	Device::Device()
	{
		m_resource_mgr = make_unique<D3D12::Resource_manager>(*this);
	}

	Device::~Device()
	{
		wait_for_gpu();

		for (auto&& frame_resource : m_frame_resource_list) {
			frame_resource->destroy();
		}

		m_resource_mgr.reset();

		m_allocator->Release();
		m_allocator = nullptr;

		// Cleanup
		ImGui_ImplDX12_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();
	}

	void Device::LoadPipeline(HWND hwnd)
	{
		m_hWnd = hwnd;

		// enable debug layer
		{
			ComPtr<ID3D12Debug> dbgController;
			if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&dbgController)))) {
				dbgController->EnableDebugLayer();
			}
		}

		ComPtr<IDXGIFactory4> factory;
		DBG::throw_hr(CreateDXGIFactory1(IID_PPV_ARGS(&factory)));

		FindHardwareAdapter(*factory.Get());
		if (!m_adapter.Get()) {
			DBG::OutputString("Failed to Find D3D12 Supported HW, Creating SW Device...");
			factory->EnumWarpAdapter(IID_PPV_ARGS(&m_adapter));
		}
		m_dxgiFactory = factory;

		// create device
		DBG::throw_hr(D3D12CreateDevice(m_adapter.Get(), REQUESTED_FEATURE_LEVEL, IID_PPV_ARGS(&m_device)));

		// create mem allocator
		D3D12MA::ALLOCATOR_DESC allocatorDesc = {};
		allocatorDesc.pDevice				  = m_device.Get();
		allocatorDesc.pAdapter				  = m_adapter.Get();

		DBG::throw_hr(D3D12MA::CreateAllocator(&allocatorDesc, &m_allocator));

		// create fence
		DBG::throw_hr(m_device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence)));

		// create a graphic command list
		CreateCommandObjects();
		// create a swap chain
		CreateSwapChain();
		// create rtv and dsv heap
		create_rtv_and_dsv_descriptor_heaps();
		create_srv_descriptor_heap();
		create_sampler_descriptor_heap();

		// create rtv and dsv
		create_backbuffer();

		// imgui now using the same srv heap as the main app - need to be intiialized after this function create_srv_descriptor_heap()
		imgui_init();
	}

	void Device::OnResize()
	{
		assert(m_device);
		assert(m_swapChain);
		// assert(m_commandAlloc);

		// Flush before changing any resources.
		flush_command_queue();

		create_backbuffer();
	}

	void Device::begin_load_resources()
	{
		// Reset the command list to prep for initialization commands.
		// D3D12 Reset() method, https://msdn.microsoft.com/en-us/library/windows/desktop/dn903895(v=vs.85).aspx
		// DBG::throw_hr(m_commandList->Reset(m_commandAlloc.Get(), nullptr));
		reset_current_command_allocator();
	}

	void Device::end_load_resources()
	{
		// Execute the initialization commands.
		// DBG::throw_hr(m_commandList->Close());
		// ID3D12CommandList* cmdsLists[] = { m_commandList.Get() };
		// m_commandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

		// Wait until initialization is complete.
		flush_command_queue();
	}

	void Device::begin_frame()
	{
		// Cycle through the circular frame resource array.
		m_curr_frame_resource_index = (m_curr_frame_resource_index + 1) % m_num_frame_resources;

		// Has the GPU finished processing the commands of the current frame resource?
		// If not, wait until the GPU has completed commands up to this fence point.
		auto&& curr_frame_resource = frame_resource();
		if (curr_frame_resource.m_fence != 0 && m_fence->GetCompletedValue() < curr_frame_resource.m_fence) {
			HANDLE event_handle = CreateEventEx(nullptr, nullptr, 0, EVENT_ALL_ACCESS);
			DBG::throw_hr(m_fence->SetEventOnCompletion(curr_frame_resource.m_fence, event_handle));
			WaitForSingleObject(event_handle, INFINITE);

			CloseHandle(event_handle);
		}

		auto&& cmd_list_allocator = curr_frame_resource.m_command_list_allocator;
		// Reuse the memory associated with command recording.
		// We can only reset when the associated command lists have finished execution on the GPU.
		DBG::throw_hr(cmd_list_allocator->Reset());

		// A command list can be reset after it has been added to the command queue via ExecuteCommandList.
		// Reusing the command list reuses memory.
		m_commandList.reset(cmd_list_allocator.Get(), nullptr);

		// set descriptor heaps for SRV
		auto&&				  curr_srv_heap		 = curr_frame_resource.m_srv_heap;
		ID3D12DescriptorHeap* descriptor_heaps[] = {curr_srv_heap.descriptor_heap(), m_sampler_heap.descriptor_heap()};
		m_commandList()->SetDescriptorHeaps(_countof(descriptor_heaps), descriptor_heaps);

		// clear statging buffers
		// reset cbv_srv_uav descriptor heap
		curr_frame_resource.begin_frame();
	}

	void Device::end_frame()
	{
		// Done recording commands.
		m_commandList.close();
		// Add the command list to the queue for execution.
		ID3D12CommandList* cmdsLists[] = {m_commandList()};
		m_commandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

		// Swap the back and front buffers
		DBG::throw_hr(m_swapChain->Present(0, 0));
		m_curr_swap_chain_buffer = (m_curr_swap_chain_buffer + 1) % m_num_swap_chain_buffer_count;

		// Advance the fence value to mark commands up to this fence point.
		auto&& curr_frame_resource	= frame_resource();
		curr_frame_resource.m_fence = ++m_currFence;

		// Add an instruction to the command queue to set a new fence point.
		// Because we are on the GPU timeline, the new fence point won't be
		// set until the GPU finishes processing all the commands prior to this Signal().
		m_commandQueue->Signal(m_fence.Get(), m_currFence);
	}

	D3D12_VIEWPORT Device::get_window_viewport() const
	{
		CRect client_rect;
		GetClientRect(m_hWnd, &client_rect);

		D3D12_VIEWPORT output;
		output.TopLeftX = (float)client_rect.TopLeft().x;
		output.TopLeftY = (float)client_rect.TopLeft().y;
		output.Width	= (float)client_rect.Width();
		output.Height	= (float)client_rect.Height();
		output.MinDepth = 0.0f;
		output.MaxDepth = 1.0f;

		return output;
	}

	CD3DX12_RECT Device::get_window_rect() const
	{
		CRect client_rect;
		GetClientRect(m_hWnd, &client_rect);
		return CD3DX12_RECT(client_rect.TopLeft().x, client_rect.TopLeft().y, client_rect.BottomRight().x, client_rect.BottomRight().y);
	}

	D3D12_CPU_DESCRIPTOR_HANDLE Device::curr_backbuffer_depth_stencil_view() const
	{
		return m_dsv_heap.descriptor_heap()->GetCPUDescriptorHandleForHeapStart();
	}

	DXGI_SWAP_CHAIN_DESC Device::get_swap_chain_desc() const
	{
		DXGI_SWAP_CHAIN_DESC desc;
		m_swapChain->GetDesc(&desc);

		return desc;
	}

	void Device::wait_for_gpu()
	{
		// Wait until the GPU has completed commands up to this fence point.
		if (m_fence->GetCompletedValue() < m_currFence) {
			HANDLE eventHandle = CreateEventEx(nullptr, nullptr, 0, EVENT_ALL_ACCESS);

			// Fire event when GPU hits current fence.
			DBG::throw_hr(m_fence->SetEventOnCompletion(m_currFence, eventHandle));

			// Wait until the GPU hits current fence event is fired.
			WaitForSingleObject(eventHandle, INFINITE);
			CloseHandle(eventHandle);
		}
	}

	void Device::flush_command_queue()
	{
		// Done recording commands.
		m_commandList.close();
		// Add the command list to the queue for execution.
		ID3D12CommandList* cmdsLists[] = {m_commandList()};
		m_commandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

		// Advance the fence value to mark commands up to this fence point.
		auto&& curr_frame_resource	= frame_resource();
		curr_frame_resource.m_fence = ++m_currFence;

		// Add an instruction to the command queue to set a new fence point.  Because we
		// are on the GPU time line, the new fence point won't be set until the GPU finishes
		// processing all the commands prior to this Signal().
		DBG::throw_hr(m_commandQueue->Signal(m_fence.Get(), m_currFence));

		// Wait until the GPU has completed commands up to this fence point.
		if (m_fence->GetCompletedValue() < m_currFence) {
			HANDLE eventHandle = CreateEventEx(nullptr, nullptr, 0, EVENT_ALL_ACCESS);

			// Fire event when GPU hits current fence.
			DBG::throw_hr(m_fence->SetEventOnCompletion(m_currFence, eventHandle));

			// Wait until the GPU hits current fence event is fired.
			WaitForSingleObject(eventHandle, INFINITE);
			CloseHandle(eventHandle);
		}
	}

	// shared_ptr<Buffer> Device::create_cbuffer(uint32_t size, const string& name)
	//{
	//     D3D12MA::ALLOCATION_DESC allocation_desc = {};
	//     allocation_desc.HeapType                 = D3D12_HEAP_TYPE_UPLOAD;
	//
	//     D3D12_RESOURCE_DESC desc;
	//     desc.Dimension          = D3D12_RESOURCE_DIMENSION_BUFFER;
	//     desc.Alignment          = 0;
	//     desc.Width              = Align_up(size, (uint32_t)D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT);
	//     desc.Height             = 1;
	//     desc.DepthOrArraySize   = 1;
	//     desc.MipLevels          = 1;
	//     desc.Format             = DXGI_FORMAT_UNKNOWN;
	//     desc.SampleDesc.Count   = 1;
	//     desc.SampleDesc.Quality = 0;
	//     desc.Layout             = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	//     desc.Flags              = D3D12_RESOURCE_FLAG_NONE;
	//
	//     auto&& buffer = std::make_shared<Buffer>();
	//     DBG::throw_hr(m_allocator->CreateResource(
	//         &allocation_desc, &desc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, &buffer->m_allocation, IID_PPV_ARGS(&buffer->m_buffer)));
	//
	//     auto&& w_name = wstring(name.begin(), name.end());
	//     buffer->m_buffer->SetName(w_name.c_str());
	//     buffer->m_d3d_desc = desc;
	//
	//     return buffer;
	// }
	//
	// shared_ptr<Dynamic_buffer> Device::create_dynamic_cbuffer(uint32_t size, const string& name)
	//{
	//     auto&& dynamic_buffer = std::make_shared<Dynamic_buffer>();
	//
	//     for (auto&& frame_resource : m_frame_resource_list) {
	//         auto&& buffer = create_cbuffer(size, name);
	//         // buffer owned by frame resource
	//         frame_resource->m_dynamic_buffer.emplace_back(buffer);
	//
	//         // this is dynamic buffer, so expecting CPU WRITE. Let persistent mapping
	//         static const CD3DX12_RANGE empty_range(0u, 0u);
	//         void*                      data = nullptr;
	//         buffer->m_buffer->Map(0, &empty_range, &data);
	//
	//         dynamic_buffer->m_data.emplace_back(data);
	//         dynamic_buffer->m_buffer.emplace_back(buffer);
	//     }
	//
	//     return dynamic_buffer;
	// }
	//
	// void* Device::get_mapped_data(const Dynamic_buffer& buffer)
	//{
	//     return buffer.m_data[m_curr_frame_resource_index];
	// }

	// weak_ptr<Buffer> Device::get_buffer(const Dynamic_buffer& buffer)
	//{
	//     return buffer.m_buffer[m_curr_frame_resource_index];
	// }

	tuple<weak_ptr<Buffer>, void*> Device::create_cbuffer(uint32_t size, const string& name)
	{
		D3D12MA::ALLOCATION_DESC allocation_desc = {};
		allocation_desc.HeapType				 = D3D12_HEAP_TYPE_UPLOAD;

		uint32_t buffer_size   = Align_up(size, (uint32_t)D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT);
		auto&&	 resource_desc = CD3DX12_RESOURCE_DESC::Buffer(buffer_size);

		auto&& buffer = std::make_shared<Buffer>();
		DBG::throw_hr(m_allocator->CreateResource(
			&allocation_desc, &resource_desc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, &buffer->m_allocation, IID_PPV_ARGS(&buffer->m_buffer)));

		auto&& w_name = wstring(name.begin(), name.end());
		buffer->m_buffer->SetName(w_name.c_str());
		buffer->m_d3d_desc	 = resource_desc;
		buffer->m_curr_state = D3D12_RESOURCE_STATE_GENERIC_READ;

		// store as a per frame object
		frame_resource().m_per_frame_buffers.emplace_back(buffer);

		// this is dynamic buffer, so expecting CPU WRITE. Let persistent mapping
		static const CD3DX12_RANGE empty_range(0u, 0u);
		void*					   data = nullptr;
		buffer->m_buffer->Map(0, &empty_range, &data);

		return make_tuple(buffer, data);
	}

	std::tuple<bool, CD3DX12_GPU_DESCRIPTOR_HANDLE> Device::get_gpu_descriptor_handle(weak_ptr<Sampler> handle)
	{
		auto&& resource = handle.lock();
		if (resource) {
			auto&& descriptor_handle_id = resource->m_handle_id;
			return std::make_tuple(true, m_sampler_heap.get_gpu_descriptor(descriptor_handle_id));
		}

		return std::make_tuple(false, CD3DX12_GPU_DESCRIPTOR_HANDLE());
	}

	std::tuple<bool, CD3DX12_CPU_DESCRIPTOR_HANDLE> Device::get_rtv_cpu_descriptor_handle(weak_ptr<Buffer> buffer_handle)
	{
		auto&& buffer = buffer_handle.lock();
		if (buffer) {
			auto&& descriptor_handle_id = buffer->m_rtv_handle_id;
			return std::make_tuple(true, m_rtv_heap.get_cpu_descriptor(descriptor_handle_id));
		}

		return std::make_tuple(false, CD3DX12_CPU_DESCRIPTOR_HANDLE());
	}

	std::tuple<bool, CD3DX12_CPU_DESCRIPTOR_HANDLE> Device::get_dsv_cpu_descriptor_handle(weak_ptr<Buffer> buffer_handle)
	{
		auto&& buffer = buffer_handle.lock();
		if (buffer) {
			auto&& descriptor_handle_id = buffer->m_dsv_handle_id;
			return std::make_tuple(true, m_dsv_heap.get_cpu_descriptor(descriptor_handle_id));
		}

		return std::make_tuple(false, CD3DX12_CPU_DESCRIPTOR_HANDLE());
	}

	// void Device::buffer_state_transition(Buffer& buffer, D3D12_RESOURCE_STATES state_before, D3D12_RESOURCE_STATES state_after)
	//{
	//     m_commandList()->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(buffer.m_buffer.Get(), state_before, state_after));
	// }

	void Device::buffer_state_transition(Buffer& buffer, D3D12_RESOURCE_STATES state_after)
	{
		D3D12_RESOURCE_STATES state_before = buffer.m_curr_state;
		if (state_before != state_after) {

			auto&& barrier = CD3DX12_RESOURCE_BARRIER::Transition(buffer.m_buffer.Get(), state_before, state_after);
			m_commandList()->ResourceBarrier(1, &barrier);
			buffer.m_curr_state = state_after;
		}
	}

	// void Device::transfer_to_back_buffer(Buffer& buffer, D3D12_RESOURCE_STATES state_before)
	//{
	//     buffer_state_transition(buffer, state_before, D3D12_RESOURCE_STATE_COPY_SOURCE);
	//     m_commandList()->ResourceBarrier(
	//         1, &CD3DX12_RESOURCE_BARRIER::Transition(&curr_backbuffer(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_COPY_DEST));
	//
	//     // copy resource
	//     m_commandList()->CopyResource(&curr_backbuffer(), buffer.m_buffer.Get());
	//
	//     buffer_state_transition(buffer, D3D12_RESOURCE_STATE_COPY_SOURCE, state_before);
	//     m_commandList()->ResourceBarrier(
	//         1, &CD3DX12_RESOURCE_BARRIER::Transition(&curr_backbuffer(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_RENDER_TARGET));
	// }

	void Device::transfer_to_back_buffer(Buffer& buffer)
	{
		buffer_state_transition(buffer, D3D12_RESOURCE_STATE_COPY_SOURCE);
		auto&& barrier = CD3DX12_RESOURCE_BARRIER::Transition(&curr_backbuffer(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_COPY_DEST);
		m_commandList()->ResourceBarrier(1, &barrier);

		// copy resource
		m_commandList()->CopyResource(&curr_backbuffer(), buffer.m_buffer.Get());

		barrier = CD3DX12_RESOURCE_BARRIER::Transition(&curr_backbuffer(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_RENDER_TARGET);
		m_commandList()->ResourceBarrier(1, &barrier);
	}

	void Device::reset_current_command_allocator()
	{
		auto&& curr_frame_resource = frame_resource();
		auto&& cmd_list_allocator  = curr_frame_resource.m_command_list_allocator;
		// Reuse the memory associated with command recording.
		// We can only reset when the associated command lists have finished execution on the GPU.
		DBG::throw_hr(cmd_list_allocator->Reset());

		// A command list can be reset after it has been added to the command queue via ExecuteCommandList.
		// Reusing the command list reuses memory.
		m_commandList.reset(cmd_list_allocator.Get(), nullptr);
	}

	void Device::build_frame_resource_list()
	{
		for (int i = 0; i < m_num_frame_resources; ++i) {
			m_frame_resource_list.push_back(make_unique<FrameResource>(m_device.Get()));
		}
	}

	void Device::create_backbuffer()
	{
		// DBG::throw_hr(m_commandList->Reset(m_commandAlloc.Get(), nullptr));
		reset_current_command_allocator();

		// Release the previous resources we will be recreating.
		for (auto&& buffer : m_swap_chain_buffer) {
			buffer.Reset();
		}
		m_resource_mgr->deregister_buffer(m_depth_buffer_id);

		CRect clientRect;
		GetClientRect(m_hWnd, &clientRect);
		// Resize the swap chain.
		DBG::throw_hr(m_swapChain->ResizeBuffers(
			m_num_swap_chain_buffer_count, clientRect.Width(), clientRect.Height(), DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH));

		m_curr_swap_chain_buffer = 0;

		for (int i = 0; i < m_num_swap_chain_buffer_count; ++i) {
			m_swap_chain_buffer_descriptor_handle_id[i] = m_rtv_heap.get_next_decriptor_id();
			auto descriptor								= m_rtv_heap.get_cpu_descriptor(m_swap_chain_buffer_descriptor_handle_id[i]);
			DBG::throw_hr(m_swapChain->GetBuffer(i, IID_PPV_ARGS(&m_swap_chain_buffer[i])));
			m_device->CreateRenderTargetView(m_swap_chain_buffer[i].Get(), nullptr, descriptor);
		}

		// resize Depth Buffer
		auto&& width  = clientRect.Width();
		auto&& height = clientRect.Height();
		// the depth buffer.  because we need to create two views to the same resource:
		//   1. SRV format: DXGI_FORMAT_R24_UNORM_X8_TYPELESS
		//   2. DSV Format: DXGI_FORMAT_D24_UNORM_S8_UINT
		// we need to create the depth buffer resource with a typeless format.
		auto&& resource_desc = CD3DX12_RESOURCE_DESC::Tex2D(m_depth_stencil_format, width, height);
		resource_desc.Flags	 = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

		CD3DX12_CLEAR_VALUE clear_val;
		clear_val.Format			   = m_depth_stencil_format;
		clear_val.DepthStencil.Depth   = 1.0f;
		clear_val.DepthStencil.Stencil = 0;

		// Create descriptor to mip level 0 of entire resource using the format of the resource.
		auto&& depth_buffer = m_resource_mgr->create_texture(m_depth_buffer_id, resource_desc, &clear_val, nullptr, D3D12_RESOURCE_STATE_DEPTH_WRITE);
		m_resource_mgr->register_buffer(m_depth_buffer_id, depth_buffer);

		// buffer_state_transition(*depth_buffer, D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_DEPTH_WRITE);

		m_resource_mgr->create_dsv(*depth_buffer, resource_desc);

		// Wait until resize is complete.
		flush_command_queue();

		// Update the viewport transform to cover the client area.
		m_screenViewport.TopLeftX = 0;
		m_screenViewport.TopLeftY = 0;
		m_screenViewport.Width	  = static_cast<float>(clientRect.Width());
		m_screenViewport.Height	  = static_cast<float>(clientRect.Height());
		m_screenViewport.MinDepth = 0.0f;
		m_screenViewport.MaxDepth = 1.0f;

		m_screenScissorRect = {0, 0, clientRect.Width(), clientRect.Height()};
	}

	void Device::imgui_init()
	{
		// Setup Dear ImGui context
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		// Setup Dear ImGui style
		ImGui::StyleColorsDark();
		// ImGui::StyleColorsClassic();

		// Setup Platform/Renderer backends
		ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
		ImGui_ImplWin32_Init(m_hWnd);

		// get an srv descriptor from static pool
		m_imgui_srv_descriptor_id = m_static_srv_heap.get_next_decriptor_id();

		auto&& cpu_descriptor_handle = m_static_srv_heap.get_cpu_descriptor(m_imgui_srv_descriptor_id);
		auto&& gpu_descriptor_handle = m_static_srv_heap.get_gpu_descriptor(m_imgui_srv_descriptor_id);

		ImGui_ImplDX12_Init(
			m_device.Get(), m_num_frame_resources, DXGI_FORMAT_R8G8B8A8_UNORM, m_srv_heap_resource.Get(), cpu_descriptor_handle, gpu_descriptor_handle);
	}

	void Device::imgui_begin_frame()
	{
		// Start the Dear ImGui frame
		ImGui_ImplDX12_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
	}

	void Device::imgui_render()
	{
		auto&& rtv = curr_backbuffer_view();
		auto&& dsv = curr_backbuffer_depth_stencil_view();
		m_commandList()->OMSetRenderTargets(1, &rtv, true, &dsv);

		ID3D12DescriptorHeap* heaps[] = {m_srv_heap_resource.Get()};

		commmand_list()()->SetDescriptorHeaps(1, heaps);
		ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), commmand_list()());
	}

	void Device::imgui_post_render()
	{
		// Update and Render additional Platform Windows
		if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
		}
	}

	void Device::FindHardwareAdapter(IDXGIFactory4& factory)
	{
		ComPtr<IDXGIAdapter1> adapter;

		for (uint32_t i = 0; DXGI_ERROR_NOT_FOUND != factory.EnumAdapters1(i, &adapter); ++i) {
			DXGI_ADAPTER_DESC1 desc;
			adapter->GetDesc1(&desc);

			if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
				continue;

			// test D3D12 device creation, no actual device created yet
			if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), REQUESTED_FEATURE_LEVEL, __uuidof(ID3D12Device), nullptr))) {
				DBG::OutputString(L"Found D3D12 HW, %s \n", desc.Description);
				break;
			}
		}

		m_adapter = adapter;
	}

	void Device::CreateCommandObjects()
	{
		D3D12_COMMAND_QUEUE_DESC desc;
		::ZeroMemory(&desc, sizeof(D3D12_COMMAND_QUEUE_DESC));
		desc.Type  = D3D12_COMMAND_LIST_TYPE_DIRECT;
		desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;

		DBG::throw_hr(m_device->CreateCommandQueue(&desc, IID_PPV_ARGS(&m_commandQueue)));

		// DBG::throw_hr(m_device->CreateCommandAllocator(
		//	D3D12_COMMAND_LIST_TYPE_DIRECT,
		//	IID_PPV_ARGS(&m_commandAlloc)));
		build_frame_resource_list();

		auto&& curr_frame_resource = frame_resource();
		auto&& cmd_list_allocator  = curr_frame_resource.m_command_list_allocator;
		// Reuse the memory associated with command recording.
		// We can only reset when the associated command lists have finished execution on the GPU.
		DBG::throw_hr(cmd_list_allocator->Reset());

		ComPtr<ID3D12GraphicsCommandList4> d3d12_command_list;
		DBG::throw_hr(m_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, cmd_list_allocator.Get(), nullptr, IID_PPV_ARGS(&d3d12_command_list)));

		// Start off in a closed state.  This is because the first time we refer
		// to the command list we will Reset it, and it needs to be closed before
		// calling Reset.
		m_commandList.init(d3d12_command_list);
	}

	void Device::CreateSwapChain()
	{
		CRect clientRect;
		GetClientRect(m_hWnd, &clientRect);

		DXGI_SWAP_CHAIN_DESC desc				= {};
		desc.BufferCount						= m_num_swap_chain_buffer_count;
		desc.BufferDesc.RefreshRate.Numerator	= 60; // Target FPS
		desc.BufferDesc.RefreshRate.Denominator = 1;
		desc.BufferDesc.Width					= clientRect.Width();
		desc.BufferDesc.Height					= clientRect.Height();
		desc.BufferDesc.Format					= DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.BufferUsage						= DXGI_USAGE_RENDER_TARGET_OUTPUT;
		desc.SwapEffect							= DXGI_SWAP_EFFECT_FLIP_DISCARD;
		desc.OutputWindow						= m_hWnd;
		desc.SampleDesc.Count					= 1;
		desc.Windowed							= true;
		desc.Flags								= DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

		ComPtr<IDXGISwapChain> swapChain;
		DBG::throw_hr(m_dxgiFactory->CreateSwapChain(m_commandQueue.Get(), // Swap chain needs the queue so that it can force a flush on it.
			&desc, &swapChain));

		DBG::throw_hr(swapChain.As(&m_swapChain));
	}

	void Device::create_srv_descriptor_heap()
	{
		// allocate srv heap
		D3D12_DESCRIPTOR_HEAP_DESC desc;
		// num_srv = n frame_resources + srv for static resources (eg ImGUI font texture)
		uint32_t num_static_srv_descriptors = m_num_imgui_srv_descriptor;
		desc.NumDescriptors					= Descriptor_heap::m_max_descriptor * m_frame_resource_list.size() + num_static_srv_descriptors;
		desc.Type							= D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		desc.Flags	  = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE; // this is where we define that this heap will provide GPUDescriptorHandle
		desc.NodeMask = 0;
		DBG::throw_hr(m_device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&m_srv_heap_resource)));

		uint32_t descriptor_size = m_device->GetDescriptorHandleIncrementSize(desc.Type);

		for (uint32_t i = 0; i < m_frame_resource_list.size(); ++i) {

			auto&& frame_resource = m_frame_resource_list[i];
			auto&& srv_heap		  = frame_resource->m_srv_heap;

			srv_heap.m_descriptor_heap	   = m_srv_heap_resource.Get();
			srv_heap.m_descriptor_size	   = descriptor_size;
			srv_heap.m_begin_descriptor_id = Descriptor_heap::m_max_descriptor * i;
		}

		// setup a static srv allocation
		{
			auto&& srv_heap = m_static_srv_heap;

			srv_heap.m_descriptor_heap	   = m_srv_heap_resource.Get();
			srv_heap.m_descriptor_size	   = descriptor_size;
			srv_heap.m_begin_descriptor_id = Descriptor_heap::m_max_descriptor * m_frame_resource_list.size();
		}
	}

	void Device::create_rtv_and_dsv_descriptor_heaps()
	{
		{
			D3D12_DESCRIPTOR_HEAP_DESC desc;
			desc.NumDescriptors = m_rtv_heap.m_max_descriptor;
			desc.Type			= D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
			desc.Flags			= D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
			desc.NodeMask		= 0;
			DBG::throw_hr(m_device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&m_rtv_heap_resource)));
		}

		{
			D3D12_DESCRIPTOR_HEAP_DESC desc;
			desc.NumDescriptors = m_dsv_heap.m_max_descriptor;
			desc.Type			= D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
			desc.Flags			= D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
			desc.NodeMask		= 0;
			DBG::throw_hr(m_device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&m_dsv_heap_resource)));
		}

		m_rtv_heap.m_descriptor_heap = m_rtv_heap_resource.Get();
		m_rtv_heap.m_descriptor_size = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		m_dsv_heap.m_descriptor_heap = m_dsv_heap_resource.Get();
		m_dsv_heap.m_descriptor_size = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
	}

	void Device::create_sampler_descriptor_heap()
	{
		D3D12_DESCRIPTOR_HEAP_DESC desc;
		desc.NumDescriptors = m_sampler_heap.m_max_descriptor;
		desc.Type			= D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;
		desc.Flags			= D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE; // this is where we define that this heap will provide GPUDescriptorHandle
		desc.NodeMask		= 0;
		DBG::throw_hr(m_device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&m_sampler_heap_resource)));

		m_sampler_heap.m_descriptor_heap = m_sampler_heap_resource.Get();
		m_sampler_heap.m_descriptor_size = m_device->GetDescriptorHandleIncrementSize(desc.Type);
	}
} // namespace D3D12
