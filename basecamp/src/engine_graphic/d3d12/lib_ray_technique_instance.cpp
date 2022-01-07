#include "lib_ray_technique_instance.h"

#include "device.h"
#include "lib_ray_shader_table.h"
#include "resource_manager.h"
#include "shader.h"
#include "shader_manager.h"

namespace D3D12 {

void Lib_ray_technique::create_ray_tracing_pipeline_state_object()
{
    // Create 7 subobjects that combine into a RTPSO:
    // Subobjects need to be associated with DXIL exports (i.e. shaders) either by way of default or explicit associations.
    // Default association applies to every exported shader entrypoint that doesn't have any of the same type of subobject associated with it.
    // This simple sample utilizes default shader association except for local root signature subobject
    // which has an explicit association specified purely for demonstration purposes.
    // 1 - DXIL library
    // 1 - Triangle hit group
    // 1 - Shader config
    // 2 - Local root signature and association
    // 1 - Global root signature
    // 1 - Pipeline config

    CD3DX12_STATE_OBJECT_DESC raytrace_pso{D3D12_STATE_OBJECT_TYPE_RAYTRACING_PIPELINE};

    // DXIL library
    // This contains the shaders and their entrypoints for the state object.
    // Since shaders are not considered a subobject, they need to be passed in via DXIL library subobjects.
    auto&& lib = raytrace_pso.CreateSubobject<CD3DX12_DXIL_LIBRARY_SUBOBJECT>();

    auto&& lib_shader      = m_shader_mgr.get_lib_shader(m_lib);
    auto&& lib_shader_blob = lib_shader->m_buffer;
    auto&& reflection      = lib_shader->m_reflection;

    D3D12_SHADER_BYTECODE libdxil = CD3DX12_SHADER_BYTECODE(lib_shader_blob->GetBufferPointer(), lib_shader_blob->GetBufferSize());
    lib->SetDXILLibrary(&libdxil);

    wstring w_raygen_entry    = To_wstring(reflection->raygen_entry);
    wstring w_miss_entry      = To_wstring(reflection->miss_entry);
    wstring w_closethit_entry = To_wstring(reflection->closethit_entry);
    wstring w_hit_group       = To_wstring(reflection->hitgroup);

    // Define which shader exports to surface from the library.
    // If no shader exports are defined for a DXIL library subobject, all shaders will be surfaced.
    // In this sample, this could be omitted for convenience since the sample uses all shaders in the library.
    {
        lib->DefineExport(w_raygen_entry.c_str());
        lib->DefineExport(w_miss_entry.c_str());
        lib->DefineExport(w_closethit_entry.c_str());
    }

    // Triangle hit group
    // A hit group specifies closest hit, any hit and intersection shaders to be executed when a ray intersects the geometry's triangle/AABB.
    // In this sample, we only use triangle geometry with a closest hit shader, so others are not set.
    auto hit_group = raytrace_pso.CreateSubobject<CD3DX12_HIT_GROUP_SUBOBJECT>();
    hit_group->SetClosestHitShaderImport(w_closethit_entry.c_str());
    hit_group->SetHitGroupExport(w_hit_group.c_str());
    hit_group->SetHitGroupType(D3D12_HIT_GROUP_TYPE_TRIANGLES);

    // Shader config
    // Defines the maximum sizes in bytes for the ray payload and attribute structure.
    auto shader_config  = raytrace_pso.CreateSubobject<CD3DX12_RAYTRACING_SHADER_CONFIG_SUBOBJECT>();
    UINT payload_size   = sizeof(XMFLOAT4); // float4 pixelColor
    UINT attribute_size = sizeof(XMFLOAT2); // float2 barycentrics
    shader_config->Config(payload_size, attribute_size);

    // [Note] use global root signature, not local
    // A littel tricky, but we can get all the info from each sub shader from Lib reflection
    //
    // Local root signature and shader association
    // This is a root signature that enables a shader to have unique arguments that come from shader tables.
    // create_root_signature_subobject(m_raygen_sub_technique, raytrace_pso, *reflection, reflection->raygen_entry);
    // create_root_signature_subobject(m_miss_sub_technique, raytrace_pso, *reflection, reflection->miss_entry);
    // create_root_signature_subobject(m_closethit_sub_technique, raytrace_pso, *reflection, reflection->closethit_entry);

    // [Note] build global root signature from combined information from func desc
    // Global root signature
    // This is a root signature that is shared across all raytracing shaders invoked during a DispatchRays() call.
    create_root_signature(raytrace_pso, *reflection);

    // Pipeline config
    // Defines the maximum TraceRay() recursion depth.
    auto pipeline_config = raytrace_pso.CreateSubobject<CD3DX12_RAYTRACING_PIPELINE_CONFIG_SUBOBJECT>();
    // PERFOMANCE TIP: Set max recursion depth as low as needed
    // as drivers may apply optimization strategies for low recursion depths.
    UINT max_recursion_depth = 1; // ~ primary rays only.
    pipeline_config->Config(max_recursion_depth);

    // Create the state object.
    auto&& device = m_shader_mgr.device().d3d_device();
    DBG::throw_hr(device->CreateStateObject(raytrace_pso, IID_PPV_ARGS(&m_dxr_state_object)));
}

// Build shader tables.
// This encapsulates all shader records - shaders and the arguments for their local root signatures.
void Lib_ray_technique::create_shader_table()
{
    auto&& lib_shader      = m_shader_mgr.get_lib_shader(m_lib);
    auto&& lib_shader_blob = lib_shader->m_buffer;
    auto&& reflection      = lib_shader->m_reflection;

    wstring w_raygen_entry    = To_wstring(reflection->raygen_entry);
    wstring w_miss_entry      = To_wstring(reflection->miss_entry);
    wstring w_closethit_entry = To_wstring(reflection->closethit_entry);

    wstring w_hit_group = To_wstring(reflection->hitgroup);

    void* raygen_shader_identifier    = nullptr;
    void* miss_shader_identifier      = nullptr;
    void* hit_group_shader_identifier = nullptr;

    // Get shader identifiers.
    UINT shader_identifier_size = D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES;
    {
        ComPtr<ID3D12StateObjectProperties> state_object_properties;
        DBG::throw_hr(m_dxr_state_object.As(&state_object_properties));

        raygen_shader_identifier    = state_object_properties->GetShaderIdentifier(w_raygen_entry.c_str());
        miss_shader_identifier      = state_object_properties->GetShaderIdentifier(w_miss_entry.c_str());
        hit_group_shader_identifier = state_object_properties->GetShaderIdentifier(w_hit_group.c_str());
    }

    auto&& resource_mgr = m_shader_mgr.device().resource_manager();
    // Ray gen shader table
    {
        // struct RootArguments {
        //     RayGenConstantBuffer cb;
        // } rootArguments;
        // rootArguments.cb = m_rayGenCB;

        // UINT numShaderRecords = 1;
        // UINT shaderRecordSize = shaderIdentifierSize + sizeof(rootArguments);
        //  ShaderTable rayGenShaderTable(device, numShaderRecords, shaderRecordSize, L"RayGenShaderTable");
        //  rayGenShaderTable.push_back(ShaderRecord(rayGenShaderIdentifier, shaderIdentifierSize, &rootArguments, sizeof(rootArguments)));
        //  m_rayGenShaderTable = rayGenShaderTable.GetResource();

        Shader_table raygen_table;
        raygen_table.m_shader_records.push_back(Shader_record(raygen_shader_identifier, shader_identifier_size));

        m_raygen_shader_table_buffer = m_lib + "_raygen_table";
        auto&& blob                  = raygen_table.generate_data();
        auto&& shader_table_buffer   = resource_mgr.create_upload_buffer(m_raygen_shader_table_buffer, (uint32_t)blob.size(), blob.data());
    }

    // Miss shader table
    {
        // UINT        numShaderRecords = 1;
        // UINT        shaderRecordSize = shaderIdentifierSize;
        // ShaderTable missShaderTable(device, numShaderRecords, shaderRecordSize, L"MissShaderTable");
        // missShaderTable.push_back(ShaderRecord(missShaderIdentifier, shaderIdentifierSize));
        // m_missShaderTable = missShaderTable.GetResource();

        Shader_table shader_table;
        shader_table.m_shader_records.push_back(Shader_record(miss_shader_identifier, shader_identifier_size));

        m_miss_shader_table_buffer = m_lib + "_miss_table";
        auto&& blob                = shader_table.generate_data();
        auto&& shader_table_buffer = resource_mgr.create_upload_buffer(m_miss_shader_table_buffer, (uint32_t)blob.size(), blob.data());
    }

    // Hit group shader table
    {
        // UINT        numShaderRecords = 1;
        // UINT        shaderRecordSize = shaderIdentifierSize;
        // ShaderTable hitGroupShaderTable(device, numShaderRecords, shaderRecordSize, L"HitGroupShaderTable");
        // hitGroupShaderTable.push_back(ShaderRecord(hitGroupShaderIdentifier, shaderIdentifierSize));
        // m_hitGroupShaderTable = hitGroupShaderTable.GetResource();

        Shader_table shader_table;
        shader_table.m_shader_records.push_back(Shader_record(hit_group_shader_identifier, shader_identifier_size));

        m_hitgroup_shader_table_buffer = m_lib + "_hitgroup_table";
        auto&& blob                    = shader_table.generate_data();
        auto&& shader_table_buffer     = resource_mgr.create_upload_buffer(m_hitgroup_shader_table_buffer, (uint32_t)blob.size(), blob.data());
    }
}

void Lib_ray_technique::create_root_signature(CD3DX12_STATE_OBJECT_DESC& raytrace_pso, Lib_ray_reflection& reflection)
{
    m_shader_mgr.build_global_root_signature(*this, reflection.get_global_input_desc());

    auto global_root_signature = raytrace_pso.CreateSubobject<CD3DX12_GLOBAL_ROOT_SIGNATURE_SUBOBJECT>();
    global_root_signature->SetRootSignature(m_root_signature.Get());
}

void Lib_ray_technique::create_root_signature_subobject(
    Lib_ray_sub_technique& sub_technique, CD3DX12_STATE_OBJECT_DESC& raytrace_pso, Lib_ray_reflection& reflection, const string& name)
{
    // build root signature
    auto&& sub_shader = reflection.get_sub_shader_info(name);
    if (!sub_shader) {
        return;
    }

    m_shader_mgr.build_local_root_signature(sub_technique, sub_shader->m_func_input_info);

    // Local root signature to be used this callable shader
    {
        auto&& localRootSignature = raytrace_pso.CreateSubobject<CD3DX12_LOCAL_ROOT_SIGNATURE_SUBOBJECT>();
        localRootSignature->SetRootSignature(sub_technique.m_root_signature.Get());
        // Shader association
        auto root_signature_association = raytrace_pso.CreateSubobject<CD3DX12_SUBOBJECT_TO_EXPORTS_ASSOCIATION_SUBOBJECT>();
        root_signature_association->SetSubobjectToAssociate(*localRootSignature);

        wstring w_name = To_wstring(sub_shader->m_name);
        root_signature_association->AddExport(w_name.c_str());
    }
}

void Lib_ray_technique_instance::init(const string& technique_name)
{
    m_technique_handle = m_shader_mgr.get_lib_ray_technique(technique_name);
    if (auto&& technique = m_technique_handle.lock()) {
        init_dynamic_cbuffer(technique->m_lib);
    }
}

void Lib_ray_technique_instance::set_cbv(const string& cbuffer_name, const string& var_name, void* data, uint32_t data_size)
{
    auto&& found_cbuffer_data = m_cbuffer.find(cbuffer_name);
    if (found_cbuffer_data != m_cbuffer.end()) {
        // validate variable info
        auto&& var_info = get_cbuffer_var_info(cbuffer_name, var_name);
        if (var_info) {
            if (var_info->Size == data_size) {
                auto&& mapped_buffer_data = m_device.get_mapped_data(*found_cbuffer_data->second);
                auto&& dest_data          = (char*)mapped_buffer_data + var_info->StartOffset;

                memcpy(dest_data, data, data_size);
            }
            else {
                throw;
            }
        }
    }
}

void Lib_ray_technique_instance::set_srv(const string& var_name, weak_ptr<Buffer> buffer)
{
    m_srv[var_name] = buffer;
}

void Lib_ray_technique_instance::set_uav(const string& var_name, weak_ptr<Buffer> buffer)
{
    m_uav[var_name] = buffer;
}

void Lib_ray_technique_instance::set_sampler(const string& var_name, weak_ptr<Sampler> resource)
{
    m_samplers[var_name] = resource;
}

void Lib_ray_technique_instance::set_root_signature_parameters(ID3D12GraphicsCommandList& command_list)
{
    auto&& technique = m_technique_handle.lock();
    if (technique) {

        command_list.SetComputeRootSignature(technique->m_root_signature.Get());
        set_root_signature_parameters(command_list, *technique);
    }
}

void Lib_ray_technique_instance::init_dynamic_cbuffer(const string& name)
{
    auto&& shader = m_shader_mgr.get_lib_shader(name);
    if (!shader) {
        return;
    }

    auto&& shader_info = shader->m_reflection;

    auto&& cbuffer_bindings = shader_info->get_global_input_desc().cbuffer_binding_desc();
    for (auto&& cbuffer_binding : cbuffer_bindings) {
        string name         = cbuffer_binding.Name;
        auto&& cbuffer_desc = shader_info->get_global_input_desc().get_cbuffer_desc(name);
        if (cbuffer_desc) {
            // only add if it is not in the list
            if (m_cbuffer.find(name) == m_cbuffer.end()) {
                auto&& cbuffer_data = m_device.create_dynamic_cbuffer(cbuffer_desc->m_desc.Size, name);
                m_cbuffer.insert(std::make_pair(name, cbuffer_data));
                m_cbuffer_infos.insert(std::make_pair(name, cbuffer_desc));
            }
        }
    }
}
const D3D12_SHADER_VARIABLE_DESC* Lib_ray_technique_instance::get_cbuffer_var_info(const string& cbuffer_name, const string& var_name)
{
    auto&& found_cbuffer_info = m_cbuffer_infos.find(cbuffer_name);
    if (found_cbuffer_info != m_cbuffer_infos.end()) {

        auto&& cbuffer_desc = found_cbuffer_info->second;

        auto&& found_var = cbuffer_desc->m_variable_infos.find(var_name);
        if (found_var != cbuffer_desc->m_variable_infos.end()) {
            return &found_var->second;
        }
    }

    return nullptr;
}
void Lib_ray_technique_instance::set_root_signature_parameters(ID3D12GraphicsCommandList& command_list, Lib_ray_technique& technique)
{
    auto&& resource_mgr = m_device.resource_manager();

    for (uint32_t i = 0; i < technique.m_descriptor_ranges.size(); ++i) {
        auto&& name = technique.m_descriptor_table_names[i];

        auto&& found_cbuffer_data = m_cbuffer.find(name);
        if (found_cbuffer_data != m_cbuffer.end()) {
            auto&& buffer = m_device.get_buffer(*found_cbuffer_data->second).lock();
            if (buffer) {
                auto&& gpu_descriptor_handle = resource_mgr.create_cbv(*buffer);
                command_list.SetComputeRootDescriptorTable(i, gpu_descriptor_handle);
            }
        }

        auto&& found_srv_data = m_srv.find(name);
        if (found_srv_data != m_srv.end()) {
            auto&& buffer = found_srv_data->second.lock();
            if (buffer) {
                auto&& gpu_descriptor_handle = resource_mgr.create_srv(*buffer);
                command_list.SetComputeRootDescriptorTable(i, gpu_descriptor_handle);
            }
        }

        auto&& found_uav_data = m_uav.find(name);
        if (found_uav_data != m_uav.end()) {
            auto&& buffer = found_uav_data->second.lock();
            if (buffer) {
                auto&& gpu_descriptor_handle = resource_mgr.create_uav(*buffer);
                command_list.SetComputeRootDescriptorTable(i, gpu_descriptor_handle);
            }
        }

        auto&& found_sampler_data = m_samplers.find(name);
        if (found_sampler_data != m_samplers.end()) {
            auto&& gpu_descriptor_handle = m_device.get_gpu_descriptor_handle(found_sampler_data->second);
            if (std::get<bool>(gpu_descriptor_handle)) {
                command_list.SetComputeRootDescriptorTable(i, std::get<CD3DX12_GPU_DESCRIPTOR_HANDLE>(gpu_descriptor_handle));
            }
        }
    }
}
} // namespace D3D12
