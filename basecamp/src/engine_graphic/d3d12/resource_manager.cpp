

#include "resource_manager.h"
#include "common/common_cpp.h"
#include "engine_graphic_cpp.h"

namespace D3D12 {
std::shared_ptr<Buffer> Resource_manager::create_static_buffer(const string& name, uint32_t byte_size, const void* init_data)
{

    auto&& resource_desc = CD3DX12_RESOURCE_DESC::Buffer(byte_size);

    D3D12MA::ALLOCATION_DESC allocation_desc = {};
    allocation_desc.HeapType                 = D3D12_HEAP_TYPE_DEFAULT;

    auto&& buffer = std::make_shared<Buffer>();
    m_device.m_allocator->CreateResource(
        &allocation_desc, &resource_desc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, &buffer->m_allocation, IID_PPV_ARGS(&buffer->m_buffer));
    {
        auto&& w_name = wstring(name.begin(), name.end());
        buffer->m_buffer->SetName(w_name.c_str());
        buffer->m_d3d_desc   = resource_desc;
        buffer->m_curr_state = D3D12_RESOURCE_STATE_GENERIC_READ;
    }

    m_static_buffers.insert(std::make_pair(name, buffer));

    auto&& command_list = m_device.commmand_list()();
    // optional, if we want to upload an initial_data
    if (init_data) {
        D3D12MA::ALLOCATION_DESC staging_allocation_desc = {};
        allocation_desc.HeapType                         = D3D12_HEAP_TYPE_UPLOAD;

        auto&& staging_buffer = std::make_shared<Buffer>();
        m_device.m_allocator->CreateResource(&allocation_desc, &resource_desc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, &staging_buffer->m_allocation,
            IID_PPV_ARGS(&staging_buffer->m_buffer));

        {
            auto&& w_name = wstring(name.begin(), name.end()) + L"_staging";
            staging_buffer->m_buffer->SetName(w_name.c_str());
        }

        // Describe the data we want to copy into the default buffer.
        D3D12_SUBRESOURCE_DATA sub_resource_data = {};
        sub_resource_data.pData                  = init_data;
        sub_resource_data.RowPitch               = byte_size;
        sub_resource_data.SlicePitch             = sub_resource_data.RowPitch;

        // Schedule to copy the data to the default buffer resource.  At a high level, the helper function UpdateSubresources
        // will copy the CPU memory into the intermediate upload heap.  Then, using ID3D12CommandList::CopySubresourceRegion,
        // the intermediate upload heap data will be copied to mBuffer.
        command_list->ResourceBarrier(
            1, &CD3DX12_RESOURCE_BARRIER::Transition(buffer->m_buffer.Get(), D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_COPY_DEST));

        // [note] this function is not at the lowest level either,
        // so, still room to learn down there
        UpdateSubresources<1>(command_list, buffer->m_buffer.Get(), staging_buffer->m_buffer.Get(), 0, 0, 1, &sub_resource_data);

        command_list->ResourceBarrier(
            1, &CD3DX12_RESOURCE_BARRIER::Transition(buffer->m_buffer.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_GENERIC_READ));

        m_device.frame_resource().m_per_frame_buffers.emplace_back(staging_buffer);
    }

    return buffer;
}

std::shared_ptr<Buffer> Resource_manager::create_upload_buffer(const string& name, uint32_t byte_size, const void* init_data)
{
    auto&& resource_desc = CD3DX12_RESOURCE_DESC::Buffer(byte_size);

    D3D12MA::ALLOCATION_DESC allocation_desc = {};
    allocation_desc.HeapType                 = D3D12_HEAP_TYPE_UPLOAD;

    auto&& buffer = std::make_shared<Buffer>();
    m_device.m_allocator->CreateResource(
        &allocation_desc, &resource_desc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, &buffer->m_allocation, IID_PPV_ARGS(&buffer->m_buffer));
    {
        auto&& w_name = wstring(name.begin(), name.end());
        buffer->m_buffer->SetName(w_name.c_str());
        buffer->m_d3d_desc = resource_desc;
    }

    m_static_buffers.insert(std::make_pair(name, buffer));

    auto&& command_list = m_device.commmand_list()();
    // optional, if we want to upload an initial_data
    if (init_data) {
        void* p_data = nullptr;
        DBG::throw_hr((buffer->m_buffer)->Map(0, nullptr, &p_data));
        memcpy(p_data, init_data, byte_size);
        (buffer->m_buffer)->Unmap(0, nullptr);
    }

    return buffer;
}

std::shared_ptr<Buffer> Resource_manager::create_texture(
    const string& name, const CD3DX12_RESOURCE_DESC& info, const CD3DX12_CLEAR_VALUE* clear_val, const TextureData* init_data, D3D12_RESOURCE_STATES init_state)
{
    auto&& resource_desc = info;

    D3D12MA::ALLOCATION_DESC allocation_desc = {};
    allocation_desc.HeapType                 = D3D12_HEAP_TYPE_DEFAULT;

    auto&& buffer = std::make_shared<Buffer>();
    m_device.m_allocator->CreateResource(&allocation_desc, &resource_desc, init_state, clear_val, &buffer->m_allocation, IID_PPV_ARGS(&buffer->m_buffer));
    {
        auto&& w_name = wstring(name.begin(), name.end());
        buffer->m_buffer->SetName(w_name.c_str());
        buffer->m_d3d_desc   = resource_desc;
        buffer->m_curr_state = init_state;
    }

    m_static_buffers.insert(std::make_pair(name, buffer));

    auto&& command_list = m_device.commmand_list()();
    // optional, if we want to upload an initial_data
    if (init_data) {
        D3D12MA::ALLOCATION_DESC staging_allocation_desc = {};
        allocation_desc.HeapType                         = D3D12_HEAP_TYPE_UPLOAD;

        auto&&       staging_buffer        = std::make_shared<Buffer>();
        const UINT64 upload_buffer_size    = GetRequiredIntermediateSize(buffer->m_buffer.Get(), 0, 1);
        auto&&       staging_resource_desc = CD3DX12_RESOURCE_DESC::Buffer(upload_buffer_size);
        m_device.m_allocator->CreateResource(&allocation_desc, &staging_resource_desc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
            &staging_buffer->m_allocation, IID_PPV_ARGS(&staging_buffer->m_buffer));

        {
            auto&& w_name = wstring(name.begin(), name.end()) + L"_staging";
            staging_buffer->m_buffer->SetName(w_name.c_str());
        }

        // Describe the data we want to copy into the default buffer.
        D3D12_SUBRESOURCE_DATA sub_resource_data = {};
        sub_resource_data.pData                  = init_data->m_data.data();
        sub_resource_data.RowPitch               = init_data->pixel_size_in_byte() * init_data->m_width;
        sub_resource_data.SlicePitch             = sub_resource_data.RowPitch * init_data->m_height;

        // Schedule to copy the data to the default buffer resource.  At a high level, the helper function UpdateSubresources
        // will copy the CPU memory into the intermediate upload heap.  Then, using ID3D12CommandList::CopySubresourceRegion,
        // the intermediate upload heap data will be copied to mBuffer.
        command_list->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(buffer->m_buffer.Get(), init_state, D3D12_RESOURCE_STATE_COPY_DEST));

        // [note] this function is not at the lowest level either,
        // so, still room to learn down there
        UpdateSubresources<1>(command_list, buffer->m_buffer.Get(), staging_buffer->m_buffer.Get(), 0, 0, 1, &sub_resource_data);

        command_list->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(buffer->m_buffer.Get(), D3D12_RESOURCE_STATE_COPY_DEST, init_state));

        m_device.frame_resource().m_per_frame_buffers.emplace_back(staging_buffer);
    }

    return buffer;
}

std::weak_ptr<Buffer> Resource_manager::create_buffer(const string& name, const Buffer_request& info, const void* init_data, D3D12_RESOURCE_STATES init_state)
{
    auto&& resource_desc = info.desc;

    D3D12MA::ALLOCATION_DESC allocation_desc = {};
    allocation_desc.HeapType                 = D3D12_HEAP_TYPE_DEFAULT;

    auto&& buffer = std::make_shared<Buffer>();
    m_device.m_allocator->CreateResource(&allocation_desc, &resource_desc, init_state, nullptr, &buffer->m_allocation, IID_PPV_ARGS(&buffer->m_buffer));
    {
        auto&& w_name = wstring(name.begin(), name.end());
        buffer->m_buffer->SetName(w_name.c_str());
        buffer->m_d3d_desc      = resource_desc;
        buffer->m_curr_state    = init_state;
        buffer->m_struct_stride = info.m_struct_stride;
    }

    // lifetime
    if (info.lifetime_persistent) {
        m_static_buffers.insert(std::make_pair(name, buffer));
    }
    else {
        m_device.frame_resource().m_per_frame_buffers.emplace_back(buffer);
    }

    auto&& command_list = m_device.commmand_list()();
    // optional, if we want to upload an initial_data
    if (init_data) {
        D3D12MA::ALLOCATION_DESC staging_allocation_desc = {};
        allocation_desc.HeapType                         = D3D12_HEAP_TYPE_UPLOAD;

        auto&&       staging_buffer        = std::make_shared<Buffer>();
        const UINT64 upload_buffer_size    = GetRequiredIntermediateSize(buffer->m_buffer.Get(), 0, 1);
        auto&&       staging_resource_desc = CD3DX12_RESOURCE_DESC::Buffer(upload_buffer_size);
        m_device.m_allocator->CreateResource(&allocation_desc, &staging_resource_desc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
            &staging_buffer->m_allocation, IID_PPV_ARGS(&staging_buffer->m_buffer));

        {
            auto&& w_name = wstring(name.begin(), name.end()) + L"_staging";
            staging_buffer->m_buffer->SetName(w_name.c_str());
        }

        // Describe the data we want to copy into the default buffer.
        D3D12_SUBRESOURCE_DATA sub_resource_data = {};
        sub_resource_data.pData                  = init_data;
        sub_resource_data.RowPitch               = resource_desc.Width;
        sub_resource_data.SlicePitch             = resource_desc.Width;

        // Schedule to copy the data to the default buffer resource.  At a high level, the helper function UpdateSubresources
        // will copy the CPU memory into the intermediate upload heap.  Then, using ID3D12CommandList::CopySubresourceRegion,
        // the intermediate upload heap data will be copied to mBuffer.
        command_list->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(buffer->m_buffer.Get(), init_state, D3D12_RESOURCE_STATE_COPY_DEST));

        // [note] this function is not at the lowest level either,
        // so, still room to learn down there
        UpdateSubresources<1>(command_list, buffer->m_buffer.Get(), staging_buffer->m_buffer.Get(), 0, 0, 1, &sub_resource_data);

        command_list->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(buffer->m_buffer.Get(), D3D12_RESOURCE_STATE_COPY_DEST, init_state));

        m_device.frame_resource().m_per_frame_buffers.emplace_back(staging_buffer);
    }

    return buffer;
}

CD3DX12_GPU_DESCRIPTOR_HANDLE Resource_manager::create_cbv(Buffer& buffer)
{
    auto id     = m_device.frame_resource().m_srv_heap.get_next_decriptor_id();
    auto handle = m_device.frame_resource().m_srv_heap.get_cpu_descriptor(id);

    D3D12_CONSTANT_BUFFER_VIEW_DESC view_desc;
    view_desc.BufferLocation = buffer.m_buffer->GetGPUVirtualAddress();
    view_desc.SizeInBytes    = (uint32_t)buffer.m_d3d_desc.Width;
    m_device.d3d_device()->CreateConstantBufferView(&view_desc, handle);

    auto&& gpu_handle = m_device.frame_resource().m_srv_heap.get_gpu_descriptor(id);
    return gpu_handle;
}

CD3DX12_GPU_DESCRIPTOR_HANDLE Resource_manager::create_srv(Buffer& buffer)
{
    auto id     = m_device.frame_resource().m_srv_heap.get_next_decriptor_id();
    auto handle = m_device.frame_resource().m_srv_heap.get_cpu_descriptor(id);

    // raytrace acceleration structure using DXGI_FORMAT_UNKNOWN
    bool is_rtaccel_structure = (buffer.m_d3d_desc.Format == DXGI_FORMAT_UNKNOWN) && buffer.m_struct_stride == 0;
    bool is_structured_buffer = (buffer.m_d3d_desc.Format == DXGI_FORMAT_UNKNOWN) && buffer.m_struct_stride > 0;

    if (is_rtaccel_structure) {
        D3D12_SHADER_RESOURCE_VIEW_DESC view_desc          = {};
        view_desc.Shader4ComponentMapping                  = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        view_desc.Format                                   = DXGI_FORMAT_UNKNOWN;
        view_desc.ViewDimension                            = D3D12_SRV_DIMENSION_RAYTRACING_ACCELERATION_STRUCTURE;
        view_desc.RaytracingAccelerationStructure.Location = buffer.m_buffer->GetGPUVirtualAddress();

        m_device.m_device->CreateShaderResourceView(nullptr, &view_desc, handle);
    }
    else if (is_structured_buffer) {
        D3D12_SHADER_RESOURCE_VIEW_DESC view_desc = {};
        view_desc.Shader4ComponentMapping         = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        view_desc.Format                          = DXGI_FORMAT_UNKNOWN;
        view_desc.ViewDimension                   = D3D12_SRV_DIMENSION_BUFFER;
        view_desc.Buffer.FirstElement             = 0;
        view_desc.Buffer.StructureByteStride      = buffer.m_struct_stride;
        view_desc.Buffer.NumElements              = buffer.m_d3d_desc.Width / buffer.m_struct_stride;

        m_device.m_device->CreateShaderResourceView(buffer.m_buffer.Get(), &view_desc, handle);
    }
    else {
        auto&&                          mapped_fmt = format_to_view_mapping(buffer.m_d3d_desc.Format, true);
        D3D12_SHADER_RESOURCE_VIEW_DESC view_desc  = {};
        view_desc.Shader4ComponentMapping          = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        view_desc.Format                           = mapped_fmt;
        view_desc.ViewDimension                    = D3D12_SRV_DIMENSION_TEXTURE2D;
        view_desc.Texture2D.MipLevels              = 1;

        m_device.m_device->CreateShaderResourceView(buffer.m_buffer.Get(), &view_desc, handle);
    }

    auto&& gpu_handle = m_device.frame_resource().m_srv_heap.get_gpu_descriptor(id);
    return gpu_handle;
}

CD3DX12_GPU_DESCRIPTOR_HANDLE Resource_manager::create_uav(Buffer& buffer)
{
    auto id     = m_device.frame_resource().m_srv_heap.get_next_decriptor_id();
    auto handle = m_device.frame_resource().m_srv_heap.get_cpu_descriptor(id);

    auto&& mapped_fmt = format_to_view_mapping(buffer.m_d3d_desc.Format, true);

    D3D12_UNORDERED_ACCESS_VIEW_DESC view_desc = {};
    view_desc.Format                           = mapped_fmt;
    view_desc.ViewDimension                    = D3D12_UAV_DIMENSION_TEXTURE2D;
    view_desc.Texture2D.MipSlice               = 0;

    m_device.m_device->CreateUnorderedAccessView(buffer.m_buffer.Get(), nullptr, &view_desc, handle);

    auto&& gpu_handle = m_device.frame_resource().m_srv_heap.get_gpu_descriptor(id);
    return gpu_handle;
}

void Resource_manager::create_rtv(Buffer& buffer, const CD3DX12_RESOURCE_DESC& desc)
{
    auto id     = m_device.m_rtv_heap.get_next_decriptor_id();
    auto handle = m_device.m_rtv_heap.get_cpu_descriptor(id);

    auto&& mapped_fmt = format_to_view_mapping(desc.Format, false);

    D3D12_RENDER_TARGET_VIEW_DESC view_desc = {};
    view_desc.ViewDimension                 = D3D12_RTV_DIMENSION_TEXTURE2D;
    view_desc.Format                        = mapped_fmt;

    m_device.m_device->CreateRenderTargetView(buffer.m_buffer.Get(), &view_desc, handle);

    buffer.m_rtv_handle_id = id;
}

void Resource_manager::create_dsv(Buffer& buffer, const CD3DX12_RESOURCE_DESC& desc)
{
    auto id     = m_device.m_dsv_heap.get_next_decriptor_id();
    auto handle = m_device.m_dsv_heap.get_cpu_descriptor(id);

    auto&& mapped_fmt = format_to_view_mapping(desc.Format, false);

    D3D12_DEPTH_STENCIL_VIEW_DESC dsv_desc = {};
    dsv_desc.Flags                         = D3D12_DSV_FLAG_NONE;
    dsv_desc.ViewDimension                 = D3D12_DSV_DIMENSION_TEXTURE2D;
    dsv_desc.Format                        = mapped_fmt;

    m_device.m_device->CreateDepthStencilView(buffer.m_buffer.Get(), &dsv_desc, handle);

    buffer.m_dsv_handle_id = id;
}

std::weak_ptr<Mesh_buffer> Resource_manager::request_mesh_buffer(const string& str_id)
{
    auto&& itr = m_mesh_buffer_list.find(str_id);
    if (itr != m_mesh_buffer_list.end()) {
        return itr->second;
    }
    return std::weak_ptr<Mesh_buffer>();
}

bool Resource_manager::register_mesh_buffer(const string& str_id, const std::shared_ptr<Mesh_buffer> mesh_buffer)
{
    auto&& handle = request_mesh_buffer(str_id);
    if (handle.expired()) {
        m_mesh_buffer_list.insert(make_pair(str_id, mesh_buffer));
        return true;
    }
    return false;
}

DXGI_FORMAT Resource_manager::format_to_view_mapping(DXGI_FORMAT fmt, bool using_as_srv)
{
    switch (fmt) {
    case DXGI_FORMAT_R32_TYPELESS:
        return using_as_srv ? DXGI_FORMAT_R32_FLOAT : DXGI_FORMAT_D32_FLOAT;
    default:
        return fmt;
    }
}

std::shared_ptr<Sampler> Resource_manager::create_sampler(const string& name, const D3d12x_sampler_desc& desc)
{
    auto id     = m_device.m_sampler_heap.get_next_decriptor_id();
    auto handle = m_device.m_sampler_heap.get_cpu_descriptor(id);

    m_device.m_device->CreateSampler(&desc, handle);

    auto&& sampler = make_shared<Sampler>();

    sampler->m_d3d_desc  = desc;
    sampler->m_handle_id = id;

    return sampler;
}

std::shared_ptr<Dynamic_buffer> Resource_manager::create_instance_buffer(const string& str_id, uint32_t byte_size)
{
    auto&& create_buffer_func = [&](const string& str_id, uint32_t byte_size) -> std::shared_ptr<Buffer> {
        D3D12MA::ALLOCATION_DESC allocation_desc = {};
        allocation_desc.HeapType                 = D3D12_HEAP_TYPE_UPLOAD;

        D3D12_RESOURCE_DESC desc;
        desc.Dimension          = D3D12_RESOURCE_DIMENSION_BUFFER;
        desc.Alignment          = 0;
        desc.Width              = byte_size;
        desc.Height             = 1;
        desc.DepthOrArraySize   = 1;
        desc.MipLevels          = 1;
        desc.Format             = DXGI_FORMAT_UNKNOWN;
        desc.SampleDesc.Count   = 1;
        desc.SampleDesc.Quality = 0;
        desc.Layout             = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
        desc.Flags              = D3D12_RESOURCE_FLAG_NONE;

        auto&& buffer = std::make_shared<Buffer>();
        DBG::throw_hr(m_device.m_allocator->CreateResource(
            &allocation_desc, &desc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, &buffer->m_allocation, IID_PPV_ARGS(&buffer->m_buffer)));

        auto&& w_name = wstring(str_id.begin(), str_id.end());
        buffer->m_buffer->SetName(w_name.c_str());
        buffer->m_d3d_desc = desc;

        return buffer;
    };

    auto&& dynamic_buffer = std::make_shared<Dynamic_buffer>();

    for (auto&& frame_resource : m_device.m_frame_resource_list) {
        auto&& buffer = create_buffer_func(str_id, byte_size);
        // buffer owned by frame resource
        frame_resource->m_dynamic_buffer.emplace_back(buffer);

        // this is dynamic buffer, so expecting CPU WRITE. Let persistent mapping
        static const CD3DX12_RANGE empty_range(0u, 0u);
        void*                      data = nullptr;
        buffer->m_buffer->Map(0, &empty_range, &data);

        dynamic_buffer->m_data.emplace_back(data);
        dynamic_buffer->m_buffer.emplace_back(buffer);
    }

    // register resource
    register_resource(m_dynamic_buffers, str_id, dynamic_buffer);

    return dynamic_buffer;
}

D3D12_VERTEX_BUFFER_VIEW Resource_manager::request_instance_buffer_view(const string& str_id, uint32_t instance_data_byte_size)
{
    auto&& handle = request_resource(m_dynamic_buffers, str_id);
    if (auto&& dyn_buffer = handle.lock()) {
        auto&& buffer_handle = dyn_buffer->m_buffer[m_device.m_curr_frame_resource_index];

        if (auto&& buffer = buffer_handle.lock()) {
            D3D12_VERTEX_BUFFER_VIEW view = {};
            view.BufferLocation           = buffer->m_buffer->GetGPUVirtualAddress();
            view.SizeInBytes              = (uint32_t)buffer->m_d3d_desc.Width;
            view.StrideInBytes            = instance_data_byte_size;

            return view;
        }
    }

    return {};
}

bool Resource_manager::update_dynamic_buffer(const string& str_id, const void* data, uint32_t byte_size)
{
    auto&& handle = request_resource(m_dynamic_buffers, str_id);
    if (auto&& buffer = handle.lock()) {
        auto&& dest = buffer->m_data[m_device.m_curr_frame_resource_index];

        // update
        memcpy(dest, data, byte_size);

        return true;
    }

    return false;
}

std::shared_ptr<Buffer> Resource_manager::create_acceleration_structure(const string& name, const Mesh_buffer& mesh_buffer, bool persistent)
{
    auto&& vertex_buffer = mesh_buffer.m_vertex_buffer_handle.lock();
    auto&& index_buffer  = mesh_buffer.m_index_buffer_handle.lock();

    if (!vertex_buffer || !index_buffer) {
        return nullptr;
    }

    D3D12_RAYTRACING_GEOMETRY_DESC geometry_desc = {};
    geometry_desc.Type                           = D3D12_RAYTRACING_GEOMETRY_TYPE_TRIANGLES;
    geometry_desc.Triangles.IndexBuffer          = index_buffer->m_buffer->GetGPUVirtualAddress();
    geometry_desc.Triangles.IndexCount           = mesh_buffer.m_mesh_location.index_count;
    geometry_desc.Triangles.IndexFormat          = mesh_buffer.idx_format;
    geometry_desc.Triangles.Transform3x4         = 0;
    // geometry_desc.Triangles.VertexFormat               = DXGI_FORMAT_R32G32B32A32_FLOAT; // D3Derror <- probably not supported//
    geometry_desc.Triangles.VertexFormat               = DXGI_FORMAT_R32G32B32_FLOAT;
    geometry_desc.Triangles.VertexCount                = mesh_buffer.vb_bytes_size / mesh_buffer.vtx_bytes_stride;
    geometry_desc.Triangles.VertexBuffer.StartAddress  = vertex_buffer->m_buffer->GetGPUVirtualAddress();
    geometry_desc.Triangles.VertexBuffer.StrideInBytes = mesh_buffer.vtx_bytes_stride;

    // Mark the geometry as opaque.
    // PERFORMANCE TIP: mark geometry as opaque whenever applicable as it can enable important ray processing optimizations.
    // Note: When rays encounter opaque geometry an any hit shader will not be executed whether it is present or not.
    geometry_desc.Flags = D3D12_RAYTRACING_GEOMETRY_FLAG_OPAQUE;

    // Get required sizes for an acceleration structure.
    D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAGS  build_flags      = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_PREFER_FAST_TRACE;
    D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS top_level_inputs = {};
    top_level_inputs.DescsLayout                                          = D3D12_ELEMENTS_LAYOUT_ARRAY;
    top_level_inputs.Flags                                                = build_flags;
    top_level_inputs.NumDescs                                             = 1;
    top_level_inputs.Type                                                 = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL;

    D3D12_RAYTRACING_ACCELERATION_STRUCTURE_PREBUILD_INFO top_level_prebuild_info = {};
    m_device.d3d_device()->GetRaytracingAccelerationStructurePrebuildInfo(&top_level_inputs, &top_level_prebuild_info);

    if (top_level_prebuild_info.ResultDataMaxSizeInBytes == 0) {
        throw;
    }

    D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS bottom_level_inputs = top_level_inputs;
    bottom_level_inputs.Type                                                 = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL;
    bottom_level_inputs.pGeometryDescs                                       = &geometry_desc;

    D3D12_RAYTRACING_ACCELERATION_STRUCTURE_PREBUILD_INFO bottom_level_prebuild_info = {};
    m_device.d3d_device()->GetRaytracingAccelerationStructurePrebuildInfo(&bottom_level_inputs, &bottom_level_prebuild_info);

    if (bottom_level_prebuild_info.ResultDataMaxSizeInBytes == 0) {
        throw;
    }

    // ComPtr<ID3D12Resource> scratchResource;
    // AllocateUAVBuffer(device, max(topLevelPrebuildInfo.ScratchDataSizeInBytes, bottomLevelPrebuildInfo.ScratchDataSizeInBytes), &scratchResource,
    //     D3D12_RESOURCE_STATE_UNORDERED_ACCESS, L"ScratchResource");

    auto&& scratch_resource_buffer_size = max(top_level_prebuild_info.ResultDataMaxSizeInBytes, bottom_level_prebuild_info.ResultDataMaxSizeInBytes);
    auto&& scratch_resource_name        = name + "_scratch";

    Buffer_request scratch_resource_req;
    scratch_resource_req.desc                = CD3DX12_RESOURCE_DESC::Buffer(scratch_resource_buffer_size, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);
    scratch_resource_req.lifetime_persistent = persistent;

    auto&& scratch_buffer = create_buffer(scratch_resource_name, scratch_resource_req, nullptr, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

    // Allocate resources for acceleration structures.
    // Acceleration structures can only be placed in resources that are created in the default heap (or custom heap equivalent).
    // Default heap is OK since the application doesn’t need CPU read/write access to them.
    // The resources that will contain acceleration structures must be created in the state D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE,
    // and must have resource flag D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS. The ALLOW_UNORDERED_ACCESS requirement simply acknowledges both:
    //  - the system will be doing this type of access in its implementation of acceleration structure builds behind the scenes.
    //  - from the app point of view, synchronization of writes/reads to acceleration structures is accomplished using UAV barriers.
    std::weak_ptr<Buffer> tlas_buffer, blas_buffer;
    {
        // D3D12_RESOURCE_STATES initialResourceState = D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE;

        // AllocateUAVBuffer(device, bottomLevelPrebuildInfo.ResultDataMaxSizeInBytes, &m_bottomLevelAccelerationStructure, initialResourceState,
        //     L"BottomLevelAccelerationStructure");
        // AllocateUAVBuffer(
        //     device, topLevelPrebuildInfo.ResultDataMaxSizeInBytes, &m_topLevelAccelerationStructure, initialResourceState,
        //     L"TopLevelAccelerationStructure");

        auto&&         tlas_resource_name = name + "_tlas";
        Buffer_request tlas_resource_req;
        tlas_resource_req.desc = CD3DX12_RESOURCE_DESC::Buffer(top_level_prebuild_info.ResultDataMaxSizeInBytes, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);
        tlas_resource_req.lifetime_persistent = persistent;
        tlas_buffer = create_buffer(tlas_resource_name, tlas_resource_req, nullptr, D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE);

        auto&&         blas_resource_name = name + "_blas";
        Buffer_request blas_resource_req;
        blas_resource_req.desc = CD3DX12_RESOURCE_DESC::Buffer(bottom_level_prebuild_info.ResultDataMaxSizeInBytes, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);
        blas_resource_req.lifetime_persistent = persistent;
        blas_buffer = create_buffer(blas_resource_name, blas_resource_req, nullptr, D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE);
    }

    // Create an instance desc for the bottom-level acceleration structure.
    D3D12_RAYTRACING_INSTANCE_DESC instance_desc = {};
    instance_desc.Transform[0][0] = instance_desc.Transform[1][1] = instance_desc.Transform[2][2] = 1;
    // instance_desc.Transform[2][3]       = 5.0f; // push this instance back a little so ray can intersec!
    instance_desc.InstanceMask          = 1;
    instance_desc.AccelerationStructure = blas_buffer.lock()->m_buffer->GetGPUVirtualAddress();
    // ComPtr<ID3D12Resource> instanceDescs;
    // AllocateUploadBuffer(device, &instanceDesc, sizeof(instanceDesc), &instanceDescs, L"InstanceDescs");
    auto&&         instance_buffer_name = name + "_instance";
    Buffer_request instance_resource_req;
    instance_resource_req.desc                = CD3DX12_RESOURCE_DESC::Buffer(sizeof(instance_desc));
    instance_resource_req.lifetime_persistent = persistent;
    auto&& instance_buffer                    = create_buffer(instance_buffer_name, instance_resource_req, &instance_desc);

    // Bottom Level Acceleration Structure desc
    D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC bottom_level_build_desc = {};
    {
        bottom_level_build_desc.Inputs                           = bottom_level_inputs;
        bottom_level_build_desc.ScratchAccelerationStructureData = scratch_buffer.lock()->m_buffer->GetGPUVirtualAddress();
        bottom_level_build_desc.DestAccelerationStructureData    = blas_buffer.lock()->m_buffer->GetGPUVirtualAddress();
    }

    // Top Level Acceleration Structure desc
    D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC top_level_build_desc = {};
    {
        top_level_inputs.InstanceDescs                        = instance_buffer.lock()->m_buffer->GetGPUVirtualAddress();
        top_level_build_desc.Inputs                           = top_level_inputs;
        top_level_build_desc.ScratchAccelerationStructureData = scratch_buffer.lock()->m_buffer->GetGPUVirtualAddress();
        top_level_build_desc.DestAccelerationStructureData    = tlas_buffer.lock()->m_buffer->GetGPUVirtualAddress();
    }

    // Build acceleration structure.
    auto&& command_list = m_device.commmand_list()();
    command_list->BuildRaytracingAccelerationStructure(&bottom_level_build_desc, 0, nullptr);
    // note, this barrier is important, need to make sure blas is finished before building tlas
    command_list->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::UAV(blas_buffer.lock()->m_buffer.Get()));
    command_list->BuildRaytracingAccelerationStructure(&top_level_build_desc, 0, nullptr);

    // -------------------------
    // create srv for tlas
    // auto id     = m_device.m_srv_heap.get_next_decriptor_id();
    // auto handle = m_device.m_srv_heap.get_cpu_descriptor(id);
    //
    // D3D12_SHADER_RESOURCE_VIEW_DESC view_desc          = {};
    // view_desc.Shader4ComponentMapping                  = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    // view_desc.Format                                   = DXGI_FORMAT_UNKNOWN;
    // view_desc.ViewDimension                            = D3D12_SRV_DIMENSION_RAYTRACING_ACCELERATION_STRUCTURE;
    // view_desc.RaytracingAccelerationStructure.Location = tlas_buffer->m_buffer->GetGPUVirtualAddress();
    //
    // m_device.m_device->CreateShaderResourceView(nullptr, &view_desc, handle);
    //
    // tlas_buffer->m_cbv_srv_handle_id = id;

    return tlas_buffer.lock();
}

std::shared_ptr<Buffer> Resource_manager::create_acceleration_structure(const string& name, const Scene_data& scene_buffer, bool persistent)
{
    auto&& num_instances = scene_buffer.num_instances();
    if (num_instances == 0) {
        return nullptr;
    }

    auto&& num_meshes = scene_buffer.m_instance_transforms.size();

    vector<weak_ptr<Buffer>> blas_buffers;
    blas_buffers.reserve(num_meshes);

    for (auto&& obj : scene_buffer.m_instance_transforms) {

        // request meshes
        auto&& mesh_name   = obj.first;
        auto&& mesh_buffer = request_mesh_buffer(mesh_name).lock();

        if (!mesh_buffer) {
            throw;
        }

        auto&& vertex_buffer = mesh_buffer->m_vertex_buffer_handle.lock();
        auto&& index_buffer  = mesh_buffer->m_index_buffer_handle.lock();
        if (!vertex_buffer || !index_buffer) {
            throw;
        }

        D3D12_RAYTRACING_GEOMETRY_DESC geometry_desc = {};
        geometry_desc.Type                           = D3D12_RAYTRACING_GEOMETRY_TYPE_TRIANGLES;
        geometry_desc.Triangles.IndexBuffer          = index_buffer->m_buffer->GetGPUVirtualAddress();
        geometry_desc.Triangles.IndexCount           = mesh_buffer->m_mesh_location.index_count;
        geometry_desc.Triangles.IndexFormat          = mesh_buffer->idx_format;
        geometry_desc.Triangles.Transform3x4         = 0;
        // geometry_desc.Triangles.VertexFormat               = DXGI_FORMAT_R32G32B32A32_FLOAT; // D3Derror <- probably not supported//
        geometry_desc.Triangles.VertexFormat               = DXGI_FORMAT_R32G32B32_FLOAT;
        geometry_desc.Triangles.VertexCount                = mesh_buffer->vb_bytes_size / mesh_buffer->vtx_bytes_stride;
        geometry_desc.Triangles.VertexBuffer.StartAddress  = vertex_buffer->m_buffer->GetGPUVirtualAddress();
        geometry_desc.Triangles.VertexBuffer.StrideInBytes = mesh_buffer->vtx_bytes_stride;

        // Mark the geometry as opaque.
        // PERFORMANCE TIP: mark geometry as opaque whenever applicable as it can enable important ray processing optimizations.
        // Note: When rays encounter opaque geometry an any hit shader will not be executed whether it is present or not.
        geometry_desc.Flags = D3D12_RAYTRACING_GEOMETRY_FLAG_OPAQUE;

        // create BLAS
        // Get required sizes for an acceleration structure.
        D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS bottom_level_inputs = {};
        bottom_level_inputs.DescsLayout                                          = D3D12_ELEMENTS_LAYOUT_ARRAY;
        bottom_level_inputs.Flags                                                = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_PREFER_FAST_TRACE;
        bottom_level_inputs.NumDescs                                             = 1;
        bottom_level_inputs.Type                                                 = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL;
        bottom_level_inputs.pGeometryDescs                                       = &geometry_desc;

        D3D12_RAYTRACING_ACCELERATION_STRUCTURE_PREBUILD_INFO bottom_level_prebuild_info = {};
        m_device.d3d_device()->GetRaytracingAccelerationStructurePrebuildInfo(&bottom_level_inputs, &bottom_level_prebuild_info);

        if (bottom_level_prebuild_info.ResultDataMaxSizeInBytes == 0) {
            throw;
        }

        auto&& scratch_resource_buffer_size = bottom_level_prebuild_info.ResultDataMaxSizeInBytes;
        auto&& scratch_resource_name        = name + "_" + mesh_name + "_blas_scratch";

        Buffer_request scratch_resource_req;
        scratch_resource_req.desc                = CD3DX12_RESOURCE_DESC::Buffer(scratch_resource_buffer_size, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);
        scratch_resource_req.lifetime_persistent = persistent;

        auto&& scratch_buffer = create_buffer(scratch_resource_name, scratch_resource_req, nullptr, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

        auto&&         blas_resource_name = name + "_" + mesh_name + "_blas";
        Buffer_request blas_resource_req;
        blas_resource_req.desc = CD3DX12_RESOURCE_DESC::Buffer(bottom_level_prebuild_info.ResultDataMaxSizeInBytes, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);
        blas_resource_req.lifetime_persistent = persistent;
        auto&& blas_buffer = create_buffer(blas_resource_name, blas_resource_req, nullptr, D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE);

        // Bottom Level Acceleration Structure desc
        D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC bottom_level_build_desc = {};
        {
            bottom_level_build_desc.Inputs                           = bottom_level_inputs;
            bottom_level_build_desc.ScratchAccelerationStructureData = scratch_buffer.lock()->m_buffer->GetGPUVirtualAddress();
            bottom_level_build_desc.DestAccelerationStructureData    = blas_buffer.lock()->m_buffer->GetGPUVirtualAddress();
        }

        // Build acceleration structure.
        auto&& command_list = m_device.commmand_list()();
        command_list->BuildRaytracingAccelerationStructure(&bottom_level_build_desc, 0, nullptr);
        // note, this barrier is important, need to make sure blas is finished before building tlas
        command_list->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::UAV(blas_buffer.lock()->m_buffer.Get()));

        blas_buffers.emplace_back(blas_buffer);
    }

    // Build instance data
    vector<D3D12_RAYTRACING_INSTANCE_DESC> instance_descs;
    instance_descs.reserve(scene_buffer.num_instances());

    int blas_id = 0;
    for (auto&& obj : scene_buffer.m_instance_transforms) {

        auto&& blas_buffer = blas_buffers[blas_id].lock();
        if (!blas_buffer) {
            throw;
        }

        for (auto&& transform : obj.second) {
            // Create an instance desc for the bottom-level acceleration structure.
            D3D12_RAYTRACING_INSTANCE_DESC instance_desc = {};
            XMStoreFloat3x4(reinterpret_cast<XMFLOAT3X4*>(instance_desc.Transform), transform);
            instance_desc.InstanceMask          = 1;
            instance_desc.AccelerationStructure = blas_buffer->m_buffer->GetGPUVirtualAddress();

            instance_descs.emplace_back(instance_desc);
        }

        blas_id++;
    }

    // create an instance buffer
    auto&&         instance_buffer_name = name + "_instance";
    Buffer_request instance_resource_req;
    instance_resource_req.desc                = CD3DX12_RESOURCE_DESC::Buffer(sizeof(D3D12_RAYTRACING_INSTANCE_DESC) * instance_descs.size());
    instance_resource_req.lifetime_persistent = persistent;
    auto&& instance_buffer                    = create_buffer(instance_buffer_name, instance_resource_req, instance_descs.data());

    // Build TLAS
    D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS top_level_inputs = {};
    top_level_inputs.DescsLayout                                          = D3D12_ELEMENTS_LAYOUT_ARRAY;
    top_level_inputs.Flags                                                = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_PREFER_FAST_TRACE;
    top_level_inputs.NumDescs                                             = (uint32_t)instance_descs.size();
    top_level_inputs.Type                                                 = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL;

    D3D12_RAYTRACING_ACCELERATION_STRUCTURE_PREBUILD_INFO top_level_prebuild_info = {};
    m_device.d3d_device()->GetRaytracingAccelerationStructurePrebuildInfo(&top_level_inputs, &top_level_prebuild_info);

    if (top_level_prebuild_info.ResultDataMaxSizeInBytes == 0) {
        throw;
    }

    auto&& scratch_resource_buffer_size = top_level_prebuild_info.ResultDataMaxSizeInBytes;
    auto&& scratch_resource_name        = name + "_tlas_scratch";

    Buffer_request scratch_resource_req;
    scratch_resource_req.desc                = CD3DX12_RESOURCE_DESC::Buffer(scratch_resource_buffer_size, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);
    scratch_resource_req.lifetime_persistent = persistent;

    auto&& scratch_buffer = create_buffer(scratch_resource_name, scratch_resource_req, nullptr, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

    auto&&         tlas_resource_name = name + "_tlas";
    Buffer_request tlas_resource_req;
    tlas_resource_req.desc = CD3DX12_RESOURCE_DESC::Buffer(top_level_prebuild_info.ResultDataMaxSizeInBytes, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);
    tlas_resource_req.lifetime_persistent = persistent;
    auto&& tlas_buffer = create_buffer(tlas_resource_name, tlas_resource_req, nullptr, D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE);

    // Top Level Acceleration Structure desc
    D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC top_level_build_desc = {};
    {
        top_level_inputs.InstanceDescs                        = instance_buffer.lock()->m_buffer->GetGPUVirtualAddress();
        top_level_build_desc.Inputs                           = top_level_inputs;
        top_level_build_desc.ScratchAccelerationStructureData = scratch_buffer.lock()->m_buffer->GetGPUVirtualAddress();
        top_level_build_desc.DestAccelerationStructureData    = tlas_buffer.lock()->m_buffer->GetGPUVirtualAddress();
    }

    // Build acceleration structure.
    auto&& command_list = m_device.commmand_list()();
    // note, this barrier is important, need to make sure blas is finished before building tlas
    command_list->BuildRaytracingAccelerationStructure(&top_level_build_desc, 0, nullptr);

    return tlas_buffer.lock();
}

} // namespace D3D12
