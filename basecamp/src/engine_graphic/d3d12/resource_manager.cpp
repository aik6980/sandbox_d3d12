

#include "resource_manager.h"
#include "common/common.cpp.h"
#include "engine_graphic_cpp.h"

namespace D3D12 {
std::shared_ptr<Buffer> Resource_manager::create_static_buffer(const string& name, uint32_t byte_size, const void* init_data)
{

    auto&& resource_desc = CD3DX12_RESOURCE_DESC::Buffer(byte_size);

    D3D12MA::ALLOCATION_DESC allocation_desc = {};
    allocation_desc.HeapType                 = D3D12_HEAP_TYPE_DEFAULT;

    auto&& buffer = std::make_shared<Buffer>();
    m_device.m_allocator->CreateResource(
        &allocation_desc, &resource_desc, D3D12_RESOURCE_STATE_COMMON, nullptr, &buffer->m_allocation, IID_PPV_ARGS(&buffer->m_buffer));
    {
        auto&& w_name = wstring(name.begin(), name.end());
        buffer->m_buffer->SetName(w_name.c_str());
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
            auto&& w_name = wstring(name.begin(), name.end()) + L"-staging";
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
            1, &CD3DX12_RESOURCE_BARRIER::Transition(buffer->m_buffer.Get(), D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COPY_DEST));

        // [note] this function is not at the lowest level either,
        // so, still room to learn down there
        UpdateSubresources<1>(command_list, buffer->m_buffer.Get(), staging_buffer->m_buffer.Get(), 0, 0, 1, &sub_resource_data);

        command_list->ResourceBarrier(
            1, &CD3DX12_RESOURCE_BARRIER::Transition(buffer->m_buffer.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_GENERIC_READ));

        m_device.frame_resource().m_staging_buffers.emplace_back(staging_buffer);
    }
    else {
        command_list->ResourceBarrier(
            1, &CD3DX12_RESOURCE_BARRIER::Transition(buffer->m_buffer.Get(), D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_GENERIC_READ));
    }

    return buffer;
}

std::shared_ptr<Buffer> Resource_manager::create_texture(
    const string& name, const CD3DX12_RESOURCE_DESC& info, const CD3DX12_CLEAR_VALUE* clear_val, const TextureData* init_data)
{
    auto&& resource_desc = info;

    D3D12MA::ALLOCATION_DESC allocation_desc = {};
    allocation_desc.HeapType                 = D3D12_HEAP_TYPE_DEFAULT;

    auto&& buffer = std::make_shared<Buffer>();
    m_device.m_allocator->CreateResource(
        &allocation_desc, &resource_desc, D3D12_RESOURCE_STATE_COMMON, clear_val, &buffer->m_allocation, IID_PPV_ARGS(&buffer->m_buffer));
    {
        auto&& w_name = wstring(name.begin(), name.end());
        buffer->m_buffer->SetName(w_name.c_str());
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
            auto&& w_name = wstring(name.begin(), name.end()) + L"-staging";
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
        command_list->ResourceBarrier(
            1, &CD3DX12_RESOURCE_BARRIER::Transition(buffer->m_buffer.Get(), D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COPY_DEST));

        // [note] this function is not at the lowest level either,
        // so, still room to learn down there
        UpdateSubresources<1>(command_list, buffer->m_buffer.Get(), staging_buffer->m_buffer.Get(), 0, 0, 1, &sub_resource_data);

        command_list->ResourceBarrier(
            1, &CD3DX12_RESOURCE_BARRIER::Transition(buffer->m_buffer.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_GENERIC_READ));

        m_device.frame_resource().m_staging_buffers.emplace_back(staging_buffer);
    }
    else {
        command_list->ResourceBarrier(
            1, &CD3DX12_RESOURCE_BARRIER::Transition(buffer->m_buffer.Get(), D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_GENERIC_READ));
    }

    return buffer;
}

void Resource_manager::create_srv(Buffer& buffer, const CD3DX12_RESOURCE_DESC& desc)
{
    auto id     = m_device.m_srv_heap.get_next_decriptor_id();
    auto handle = m_device.m_srv_heap.get_cpu_descriptor(id);

    auto&& mapped_fmt = format_to_view_mapping(desc.Format, true);

    D3D12_SHADER_RESOURCE_VIEW_DESC view_desc = {};
    view_desc.Shader4ComponentMapping         = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    view_desc.Format                          = mapped_fmt;
    view_desc.ViewDimension                   = D3D12_SRV_DIMENSION_TEXTURE2D;
    view_desc.Texture2D.MipLevels             = 1;

    m_device.m_device->CreateShaderResourceView(buffer.m_buffer.Get(), &view_desc, handle);

    buffer.m_cbv_srv_handle_id = id;
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

std::weak_ptr<MESH_BUFFER> Resource_manager::request_mesh_buffer(const string& str_id)
{
    auto&& itr = m_mesh_buffer_list.find(str_id);
    if (itr != m_mesh_buffer_list.end()) {
        return itr->second;
    }
    return std::weak_ptr<MESH_BUFFER>();
}

bool Resource_manager::register_mesh_buffer(const string& str_id, const std::shared_ptr<MESH_BUFFER> mesh_buffer)
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

} // namespace D3D12
