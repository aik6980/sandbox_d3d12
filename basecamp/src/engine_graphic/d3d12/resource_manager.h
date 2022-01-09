#pragma once

#include "engine_graphic.h"

struct TextureData;

namespace D3D12 {

class Resource_manager {
  public:
    Resource_manager(Device& device) : m_device(device) {}

    ~Resource_manager() {}

    std::shared_ptr<Buffer> create_static_buffer(const string& name, uint32_t byte_size, const void* init_data);

    std::shared_ptr<Buffer> create_texture(const string& name, const CD3DX12_RESOURCE_DESC& info, const CD3DX12_CLEAR_VALUE* clear_val,
        const TextureData* init_data, D3D12_RESOURCE_STATES init_state = D3D12_RESOURCE_STATE_GENERIC_READ);

    std::weak_ptr<Buffer> create_buffer(
        const string& name, const Buffer_request& info, const void* init_data, D3D12_RESOURCE_STATES init_state = D3D12_RESOURCE_STATE_GENERIC_READ);

    // views
    CD3DX12_GPU_DESCRIPTOR_HANDLE create_cbv(Buffer& buffer);
    CD3DX12_GPU_DESCRIPTOR_HANDLE create_srv(Buffer& buffer);
    CD3DX12_GPU_DESCRIPTOR_HANDLE create_uav(Buffer& buffer);
    void                          create_rtv(Buffer& buffer, const CD3DX12_RESOURCE_DESC& desc);
    void                          create_dsv(Buffer& buffer, const CD3DX12_RESOURCE_DESC& desc);

    std::weak_ptr<Mesh_buffer> request_mesh_buffer(const string& str_id);

    bool register_mesh_buffer(const string& str_id, const std::shared_ptr<Mesh_buffer> mesh_buffer);

    std::weak_ptr<Buffer>  request_buffer(const string& str_id) { return request_resource(m_static_buffers, str_id); }
    std::weak_ptr<Sampler> request_sampler(const string& str_id) { return request_resource(m_samplers, str_id); }

    bool register_buffer(const string& str_id, const std::shared_ptr<Buffer> buffer) { return register_resource(m_static_buffers, str_id, buffer); }
    bool deregister_buffer(const string& str_id) { return deregister_resource(m_static_buffers, str_id); }

    DXGI_FORMAT format_to_view_mapping(DXGI_FORMAT fmt, bool using_as_srv);

    std::shared_ptr<Sampler> create_sampler(const string& name, const D3d12x_sampler_desc& desc);
    bool register_sampler(const string& str_id, const std::shared_ptr<Sampler> sampler) { return register_resource(m_samplers, str_id, sampler); }

    std::shared_ptr<Dynamic_buffer> create_instance_buffer(const string& str_id, uint32_t byte_size);
    D3D12_VERTEX_BUFFER_VIEW        request_instance_buffer_view(const string& str_id, uint32_t instance_data_byte_size);
    bool                            update_dynamic_buffer(const string& str_id, const void* data, uint32_t byte_size);

    // dxr
    std::shared_ptr<Buffer> create_acceleration_structure(const string& name, const Mesh_buffer& mesh_buffer, bool persistent);
    std::shared_ptr<Buffer> create_acceleration_structure(const string& name, const Scene_data& mesh_buffer, bool persistent);

  private:
    template <class T>
    std::weak_ptr<T> request_resource(unordered_map<string, std::shared_ptr<T>>& map, const string& str_id)
    {
        auto&& itr = map.find(str_id);
        if (itr != map.end()) {
            return itr->second;
        }
        return std::weak_ptr<T>();
    }

    template <class T>
    bool register_resource(unordered_map<string, std::shared_ptr<T>>& map, const string& str_id, const std::shared_ptr<T> buffer)
    {
        auto&& handle = request_mesh_buffer(str_id);
        if (handle.expired()) {
            map.insert(make_pair(str_id, buffer));
            return true;
        }
        return false;
    }

    template <class T>
    bool deregister_resource(unordered_map<string, std::shared_ptr<T>>& map, const string& str_id)
    {
        auto&& itr = map.find(str_id);
        if (itr != map.end()) {
            map.erase(itr);
            return true;
        }
        return false;
    }

    // parent
    Device& m_device;

    unordered_map<string, std::shared_ptr<Mesh_buffer>>    m_mesh_buffer_list;
    unordered_map<string, std::shared_ptr<Buffer>>         m_static_buffers;
    unordered_map<string, std::shared_ptr<Dynamic_buffer>> m_dynamic_buffers;
    unordered_map<string, std::shared_ptr<Sampler>>        m_samplers;

    // resource state trackers
    unordered_map<string, Resource_state_tracker> m_resource_state_trackers;
};
} // namespace D3D12
