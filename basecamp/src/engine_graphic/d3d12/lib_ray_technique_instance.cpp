#include "lib_ray_technique_instance.h"

#include "device.h"
#include "lib_ray_shader_table.h"
#include "resource_manager.h"
#include "shader.h"
#include "shader_manager.h"

void D3D12::Lib_ray_technique::create_ray_tracing_pipeline_state_object()
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

    wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;

    wstring w_raygen_entry    = converter.from_bytes(reflection->raygen_entry());
    wstring w_miss_entry      = converter.from_bytes(reflection->miss_entry());
    wstring w_closethit_entry = converter.from_bytes(reflection->closethit_entry());

    wstring w_hit_group = converter.from_bytes(reflection->hitgroup());
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

    // [Note] these, I can simply get from the function reflections
    // Local root signature and shader association
    // This is a root signature that enables a shader to have unique arguments that come from shader tables.
    // CreateLocalRootSignatureSubobjects(&raytracingPipeline);

    // [Note] I don't think i can get this information from reflections
    // Global root signature
    // This is a root signature that is shared across all raytracing shaders invoked during a DispatchRays() call.
    // auto globalRootSignature = raytracingPipeline.CreateSubobject<CD3DX12_GLOBAL_ROOT_SIGNATURE_SUBOBJECT>();
    // globalRootSignature->SetRootSignature(m_raytracingGlobalRootSignature.Get());

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
void D3D12::Lib_ray_technique::create_shader_table()
{
    auto&& lib_shader      = m_shader_mgr.get_lib_shader(m_lib);
    auto&& lib_shader_blob = lib_shader->m_buffer;
    auto&& reflection      = lib_shader->m_reflection;

    wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;

    wstring w_raygen_entry    = converter.from_bytes(reflection->raygen_entry());
    wstring w_miss_entry      = converter.from_bytes(reflection->miss_entry());
    wstring w_closethit_entry = converter.from_bytes(reflection->closethit_entry());

    wstring w_hit_group = converter.from_bytes(reflection->hitgroup());

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

        auto&& blob                = raygen_table.generate_data();
        auto&& shader_table_buffer = resource_mgr.create_upload_buffer(m_raygen_shader_table_buffer, blob.size(), blob.data());
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

        auto&& blob                = shader_table.generate_data();
        auto&& shader_table_buffer = resource_mgr.create_upload_buffer(m_miss_shader_table_buffer, blob.size(), blob.data());
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

        auto&& blob                = shader_table.generate_data();
        auto&& shader_table_buffer = resource_mgr.create_upload_buffer(m_hitgroup_shader_table_buffer, blob.size(), blob.data());
    }
}
