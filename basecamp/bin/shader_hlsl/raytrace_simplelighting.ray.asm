;
; Note: shader requires additional functionality:
;       UAVs at every shader stage
;
; shader hash: b4a110bc0d75b3b330c7906c6b691f9a
;
; Buffer Definitions:
;
; cbuffer Raygen_cb
; {
;
;   struct Raygen_cb
;   {
;
;       float4 Main_vp;                               ; Offset:    0
;       float4 Stencil_vp;                            ; Offset:   16
;   
;   } Raygen_cb;                                      ; Offset:    0 Size:    32
;
; }
;
; cbuffer Camera_cb
; {
;
;   struct hostlayout.Camera_cb
;   {
;
;       row_major float4x4 Camera_projection_to_world;; Offset:    0
;       float3 Camera_world_pos;                      ; Offset:   64
;   
;   } Camera_cb;                                      ; Offset:    0 Size:    76
;
; }
;
; Resource bind info for Instance_data_srv
; {
;
;   struct struct.Instance_data
;   {
;
;       uint m_mesh_id;                               ; Offset:    0
;   
;   } $Element;                                       ; Offset:    0 Size:     4
;
; }
;
; Resource bind info for Mesh_data_srv
; {
;
;   struct struct.Mesh_desc
;   {
;
;       uint m_num_vertices;                          ; Offset:    0
;       uint m_num_indices;                           ; Offset:    4
;       uint m_offset_vertices;                       ; Offset:    8
;       uint m_offset_indices;                        ; Offset:   12
;   
;   } $Element;                                       ; Offset:    0 Size:    16
;
; }
;
; Resource bind info for Vertices_srv
; {
;
;   struct struct.Fat_vertex
;   {
;
;       float3 m_position;                            ; Offset:    0
;       float4 m_colour;                              ; Offset:   12
;       float3 m_normal;                              ; Offset:   28
;   
;   } $Element;                                       ; Offset:    0 Size:    40
;
; }
;
;
; Resource Bindings:
;
; Name                                 Type  Format         Dim      ID      HLSL Bind  Count
; ------------------------------ ---------- ------- ----------- ------- -------------- ------
; Raygen_cb                         cbuffer      NA          NA     CB0            cb0     1
; Camera_cb                         cbuffer      NA          NA     CB1            cb1     1
; Scene_srv                         texture     i32         ras      T0             t0     1
; Instance_data_srv                 texture  struct         r/o      T1             t1     1
; Mesh_data_srv                     texture  struct         r/o      T2             t2     1
; Vertices_srv                      texture  struct         r/o      T3             t3     1
; Indices_srv                       texture     u32         buf      T4             t4     1
; Output_uav                            UAV     f32          2d      U0             u0     1
;
target datalayout = "e-m:e-p:32:32-i1:32-i8:32-i16:32-i32:32-i64:64-f16:32-f32:32-f64:64-n8:16:32:64"
target triple = "dxil-ms-dx"

%struct.RaytracingAccelerationStructure = type { i32 }
%"class.RWTexture2D<vector<float, 4> >" = type { <4 x float> }
%"class.StructuredBuffer<Instance_data>" = type { %struct.Instance_data }
%struct.Instance_data = type { i32 }
%"class.StructuredBuffer<Mesh_desc>" = type { %struct.Mesh_desc }
%struct.Mesh_desc = type { i32, i32, i32, i32 }
%"class.StructuredBuffer<Fat_vertex>" = type { %struct.Fat_vertex }
%struct.Fat_vertex = type { <3 x float>, <4 x float>, <3 x float> }
%"class.Buffer<unsigned int>" = type { i32 }
%Raygen_cb = type { <4 x float>, <4 x float> }
%hostlayout.Camera_cb = type { [4 x <4 x float>], <3 x float> }
%struct.RayDesc = type { <3 x float>, float, <3 x float>, float }
%struct.Payload_st = type { <4 x float> }
%dx.types.Handle = type { i8* }
%dx.types.CBufRet.f32 = type { float, float, float, float }
%struct.BuiltInTriangleIntersectionAttributes = type { <2 x float> }
%dx.types.ResRet.i32 = type { i32, i32, i32, i32, i32 }
%dx.types.ResRet.f32 = type { float, float, float, float, i32 }

@"\01?Scene_srv@@3URaytracingAccelerationStructure@@A" = external constant %struct.RaytracingAccelerationStructure, align 4
@"\01?Output_uav@@3V?$RWTexture2D@V?$vector@M$03@@@@A" = external constant %"class.RWTexture2D<vector<float, 4> >", align 4
@"\01?Instance_data_srv@@3V?$StructuredBuffer@UInstance_data@@@@A" = external constant %"class.StructuredBuffer<Instance_data>", align 4
@"\01?Mesh_data_srv@@3V?$StructuredBuffer@UMesh_desc@@@@A" = external constant %"class.StructuredBuffer<Mesh_desc>", align 4
@"\01?Vertices_srv@@3V?$StructuredBuffer@UFat_vertex@@@@A" = external constant %"class.StructuredBuffer<Fat_vertex>", align 4
@"\01?Indices_srv@@3V?$Buffer@I@@A" = external constant %"class.Buffer<unsigned int>", align 4
@Raygen_cb = external constant %Raygen_cb
@Camera_cb_legacy = external global %hostlayout.Camera_cb
@dx.nothing.a = internal constant [1 x i32] zeroinitializer

; Function Attrs: nounwind
define void @"\01?raygen_entry@@YAXXZ"() #0 {
  %1 = load %struct.RaytracingAccelerationStructure, %struct.RaytracingAccelerationStructure* @"\01?Scene_srv@@3URaytracingAccelerationStructure@@A"
  %2 = load %"class.RWTexture2D<vector<float, 4> >", %"class.RWTexture2D<vector<float, 4> >"* @"\01?Output_uav@@3V?$RWTexture2D@V?$vector@M$03@@@@A"
  %3 = load %hostlayout.Camera_cb, %hostlayout.Camera_cb* @Camera_cb_legacy
  %4 = load %Raygen_cb, %Raygen_cb* @Raygen_cb
  %5 = alloca %struct.RayDesc, align 4
  %6 = alloca %struct.Payload_st, align 4
  %7 = call %dx.types.Handle @dx.op.createHandleForLib.hostlayout.Camera_cb(i32 160, %hostlayout.Camera_cb %3)  ; CreateHandleForLib(Resource)
  %8 = call %dx.types.Handle @dx.op.createHandleForLib.Raygen_cb(i32 160, %Raygen_cb %4)  ; CreateHandleForLib(Resource)
  %9 = call i32 @dx.op.dispatchRaysIndex.i32(i32 145, i8 0)  ; DispatchRaysIndex(col)
  %10 = call i32 @dx.op.dispatchRaysIndex.i32(i32 145, i8 1)  ; DispatchRaysIndex(col)
  %11 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0)
  %12 = call i32 @dx.op.dispatchRaysDimensions.i32(i32 146, i8 0)  ; DispatchRaysDimensions(col)
  %13 = call i32 @dx.op.dispatchRaysDimensions.i32(i32 146, i8 1)  ; DispatchRaysDimensions(col)
  %14 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0)
  %15 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0)
  %16 = uitofp i32 %9 to float
  %17 = uitofp i32 %10 to float
  %18 = uitofp i32 %12 to float
  %19 = uitofp i32 %13 to float
  %20 = fdiv fast float %16, %18
  %21 = fdiv fast float %17, %19
  %22 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0)
  %23 = call %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32 59, %dx.types.Handle %8, i32 0)  ; CBufferLoadLegacy(handle,regIndex)
  %24 = extractvalue %dx.types.CBufRet.f32 %23, 0
  %25 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0)
  %26 = call %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32 59, %dx.types.Handle %8, i32 0)  ; CBufferLoadLegacy(handle,regIndex)
  %27 = extractvalue %dx.types.CBufRet.f32 %26, 1
  %28 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0)
  %29 = call %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32 59, %dx.types.Handle %8, i32 0)  ; CBufferLoadLegacy(handle,regIndex)
  %30 = extractvalue %dx.types.CBufRet.f32 %29, 2
  %31 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0)
  %32 = call %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32 59, %dx.types.Handle %8, i32 0)  ; CBufferLoadLegacy(handle,regIndex)
  %33 = extractvalue %dx.types.CBufRet.f32 %32, 3
  %34 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0)
  %35 = fsub fast float %30, %24
  %36 = fmul fast float %20, %35
  %37 = fadd fast float %24, %36
  %38 = fsub fast float %33, %27
  %39 = fmul fast float %21, %38
  %40 = fadd fast float %27, %39
  %41 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0)
  %42 = call %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32 59, %dx.types.Handle %8, i32 1)  ; CBufferLoadLegacy(handle,regIndex)
  %43 = extractvalue %dx.types.CBufRet.f32 %42, 0
  %44 = extractvalue %dx.types.CBufRet.f32 %42, 1
  %45 = extractvalue %dx.types.CBufRet.f32 %42, 2
  %46 = extractvalue %dx.types.CBufRet.f32 %42, 3
  %47 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0)
  %48 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0)
  %49 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0)
  %50 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0)
  %51 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0)
  %52 = fcmp fast oge float %37, %43
  %53 = fcmp fast ole float %37, %45
  %54 = and i1 %52, %53
  %55 = fcmp fast oge float %40, %44
  %56 = fcmp fast ole float %40, %46
  %57 = and i1 %55, %56
  %58 = and i1 %54, %57
  %59 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0)
  br i1 %58, label %60, label %179

; <label>:60                                      ; preds = %0
  %61 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0)
  %62 = uitofp i32 %9 to float
  %63 = uitofp i32 %10 to float
  %64 = fadd fast float %62, 5.000000e-01
  %65 = fadd fast float %63, 5.000000e-01
  %66 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0)
  %67 = uitofp i32 %12 to float
  %68 = uitofp i32 %13 to float
  %69 = fdiv fast float %64, %67
  %70 = fdiv fast float %65, %68
  %71 = fmul fast float %69, 2.000000e+00
  %72 = fmul fast float %70, 2.000000e+00
  %73 = fsub fast float %71, 1.000000e+00
  %74 = fsub fast float %72, 1.000000e+00
  %75 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0)
  %76 = fsub fast float -0.000000e+00, %74
  %77 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0)
  %78 = call %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32 59, %dx.types.Handle %7, i32 0)  ; CBufferLoadLegacy(handle,regIndex)
  %79 = extractvalue %dx.types.CBufRet.f32 %78, 0
  %80 = extractvalue %dx.types.CBufRet.f32 %78, 1
  %81 = extractvalue %dx.types.CBufRet.f32 %78, 2
  %82 = extractvalue %dx.types.CBufRet.f32 %78, 3
  %83 = call %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32 59, %dx.types.Handle %7, i32 1)  ; CBufferLoadLegacy(handle,regIndex)
  %84 = extractvalue %dx.types.CBufRet.f32 %83, 0
  %85 = extractvalue %dx.types.CBufRet.f32 %83, 1
  %86 = extractvalue %dx.types.CBufRet.f32 %83, 2
  %87 = extractvalue %dx.types.CBufRet.f32 %83, 3
  %88 = call %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32 59, %dx.types.Handle %7, i32 2)  ; CBufferLoadLegacy(handle,regIndex)
  %89 = extractvalue %dx.types.CBufRet.f32 %88, 0
  %90 = extractvalue %dx.types.CBufRet.f32 %88, 1
  %91 = extractvalue %dx.types.CBufRet.f32 %88, 2
  %92 = extractvalue %dx.types.CBufRet.f32 %88, 3
  %93 = call %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32 59, %dx.types.Handle %7, i32 3)  ; CBufferLoadLegacy(handle,regIndex)
  %94 = extractvalue %dx.types.CBufRet.f32 %93, 0
  %95 = extractvalue %dx.types.CBufRet.f32 %93, 1
  %96 = extractvalue %dx.types.CBufRet.f32 %93, 2
  %97 = extractvalue %dx.types.CBufRet.f32 %93, 3
  %98 = fmul fast float %73, %79
  %99 = call float @dx.op.tertiary.f32(i32 46, float %76, float %84, float %98)  ; FMad(a,b,c)
  %100 = call float @dx.op.tertiary.f32(i32 46, float 0.000000e+00, float %89, float %99)  ; FMad(a,b,c)
  %101 = call float @dx.op.tertiary.f32(i32 46, float 1.000000e+00, float %94, float %100)  ; FMad(a,b,c)
  %102 = fmul fast float %73, %80
  %103 = call float @dx.op.tertiary.f32(i32 46, float %76, float %85, float %102)  ; FMad(a,b,c)
  %104 = call float @dx.op.tertiary.f32(i32 46, float 0.000000e+00, float %90, float %103)  ; FMad(a,b,c)
  %105 = call float @dx.op.tertiary.f32(i32 46, float 1.000000e+00, float %95, float %104)  ; FMad(a,b,c)
  %106 = fmul fast float %73, %81
  %107 = call float @dx.op.tertiary.f32(i32 46, float %76, float %86, float %106)  ; FMad(a,b,c)
  %108 = call float @dx.op.tertiary.f32(i32 46, float 0.000000e+00, float %91, float %107)  ; FMad(a,b,c)
  %109 = call float @dx.op.tertiary.f32(i32 46, float 1.000000e+00, float %96, float %108)  ; FMad(a,b,c)
  %110 = fmul fast float %73, %82
  %111 = call float @dx.op.tertiary.f32(i32 46, float %76, float %87, float %110)  ; FMad(a,b,c)
  %112 = call float @dx.op.tertiary.f32(i32 46, float 0.000000e+00, float %92, float %111)  ; FMad(a,b,c)
  %113 = call float @dx.op.tertiary.f32(i32 46, float 1.000000e+00, float %97, float %112)  ; FMad(a,b,c)
  %114 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0)
  %115 = fdiv fast float %101, %113
  %116 = fdiv fast float %105, %113
  %117 = fdiv fast float %109, %113
  %118 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0)
  %119 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0)
  %120 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0)
  %121 = call %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32 59, %dx.types.Handle %7, i32 4)  ; CBufferLoadLegacy(handle,regIndex)
  %122 = extractvalue %dx.types.CBufRet.f32 %121, 0
  %123 = insertelement <3 x float> undef, float %122, i64 0
  %124 = extractvalue %dx.types.CBufRet.f32 %121, 1
  %125 = insertelement <3 x float> %123, float %124, i64 1
  %126 = extractvalue %dx.types.CBufRet.f32 %121, 2
  %127 = insertelement <3 x float> %125, float %126, i64 2
  %128 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0)
  %129 = fsub fast float %115, %122
  %130 = fsub fast float %116, %124
  %131 = fsub fast float %117, %126
  %132 = call float @dx.op.dot3.f32(i32 55, float %129, float %130, float %131, float %129, float %130, float %131)  ; Dot3(ax,ay,az,bx,by,bz)
  %133 = call float @dx.op.unary.f32(i32 25, float %132)  ; Rsqrt(value)
  %134 = fmul fast float %129, %133
  %135 = fmul fast float %130, %133
  %136 = fmul fast float %131, %133
  %137 = insertelement <3 x float> undef, float %134, i32 0
  %138 = insertelement <3 x float> %137, float %135, i32 1
  %139 = insertelement <3 x float> %138, float %136, i32 2
  %140 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0)
  %141 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0)
  %142 = getelementptr inbounds %struct.RayDesc, %struct.RayDesc* %5, i32 0, i32 0
  %143 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0)
  store <3 x float> %127, <3 x float>* %142, align 4
  %144 = getelementptr inbounds %struct.RayDesc, %struct.RayDesc* %5, i32 0, i32 2
  %145 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0)
  store <3 x float> %139, <3 x float>* %144, align 4
  %146 = getelementptr inbounds %struct.RayDesc, %struct.RayDesc* %5, i32 0, i32 1
  %147 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0)
  store float 0x3F50624DE0000000, float* %146, align 4
  %148 = getelementptr inbounds %struct.RayDesc, %struct.RayDesc* %5, i32 0, i32 3
  %149 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0)
  store float 1.000000e+04, float* %148, align 4
  %150 = getelementptr inbounds %struct.Payload_st, %struct.Payload_st* %6, i32 0, i32 0
  %151 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0)
  store <4 x float> zeroinitializer, <4 x float>* %150, align 4
  %152 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0)
  %153 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0)
  %154 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0)
  %155 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0)
  %156 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0)
  %157 = call %dx.types.Handle @dx.op.createHandleForLib.struct.RaytracingAccelerationStructure(i32 160, %struct.RaytracingAccelerationStructure %1)  ; CreateHandleForLib(Resource)
  %158 = getelementptr %struct.RayDesc, %struct.RayDesc* %5, i32 0, i32 0
  %159 = load <3 x float>, <3 x float>* %158
  %160 = extractelement <3 x float> %159, i64 0
  %161 = extractelement <3 x float> %159, i64 1
  %162 = extractelement <3 x float> %159, i64 2
  %163 = getelementptr %struct.RayDesc, %struct.RayDesc* %5, i32 0, i32 1
  %164 = load float, float* %163
  %165 = getelementptr %struct.RayDesc, %struct.RayDesc* %5, i32 0, i32 2
  %166 = load <3 x float>, <3 x float>* %165
  %167 = extractelement <3 x float> %166, i64 0
  %168 = extractelement <3 x float> %166, i64 1
  %169 = extractelement <3 x float> %166, i64 2
  %170 = getelementptr %struct.RayDesc, %struct.RayDesc* %5, i32 0, i32 3
  %171 = load float, float* %170
  call void @dx.op.traceRay.struct.Payload_st(i32 157, %dx.types.Handle %157, i32 16, i32 -1, i32 0, i32 1, i32 0, float %160, float %161, float %162, float %164, float %167, float %168, float %169, float %171, %struct.Payload_st* %6)  ; TraceRay(AccelerationStructure,RayFlags,InstanceInclusionMask,RayContributionToHitGroupIndex,MultiplierForGeometryContributionToShaderIndex,MissShaderIndex,Origin_X,Origin_Y,Origin_Z,TMin,Direction_X,Direction_Y,Direction_Z,TMax,payload)
  %172 = getelementptr inbounds %struct.Payload_st, %struct.Payload_st* %6, i32 0, i32 0
  %173 = load <4 x float>, <4 x float>* %172, align 4
  %174 = call %dx.types.Handle @"dx.op.createHandleForLib.class.RWTexture2D<vector<float, 4> >"(i32 160, %"class.RWTexture2D<vector<float, 4> >" %2)  ; CreateHandleForLib(Resource)
  %175 = extractelement <4 x float> %173, i64 0
  %176 = extractelement <4 x float> %173, i64 1
  %177 = extractelement <4 x float> %173, i64 2
  %178 = extractelement <4 x float> %173, i64 3
  call void @dx.op.textureStore.f32(i32 67, %dx.types.Handle %174, i32 %9, i32 %10, i32 undef, float %175, float %176, float %177, float %178, i8 15)  ; TextureStore(srv,coord0,coord1,coord2,value0,value1,value2,value3,mask)
  br label %182

; <label>:179                                     ; preds = %0
  %180 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0)
  %181 = call %dx.types.Handle @"dx.op.createHandleForLib.class.RWTexture2D<vector<float, 4> >"(i32 160, %"class.RWTexture2D<vector<float, 4> >" %2)  ; CreateHandleForLib(Resource)
  call void @dx.op.textureStore.f32(i32 67, %dx.types.Handle %181, i32 %9, i32 %10, i32 undef, float %20, float %21, float 0.000000e+00, float 1.000000e+00, i8 15)  ; TextureStore(srv,coord0,coord1,coord2,value0,value1,value2,value3,mask)
  br label %182

; <label>:182                                     ; preds = %179, %60
  %183 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0)
  ret void
}

; Function Attrs: nounwind
define void @"\01?closethit_entry@@YAXUPayload_st@@UBuiltInTriangleIntersectionAttributes@@@Z"(%struct.Payload_st* noalias %payload, %struct.BuiltInTriangleIntersectionAttributes* %attr) #0 {
  %1 = load %"class.Buffer<unsigned int>", %"class.Buffer<unsigned int>"* @"\01?Indices_srv@@3V?$Buffer@I@@A"
  %2 = load %"class.StructuredBuffer<Fat_vertex>", %"class.StructuredBuffer<Fat_vertex>"* @"\01?Vertices_srv@@3V?$StructuredBuffer@UFat_vertex@@@@A"
  %3 = load %"class.StructuredBuffer<Mesh_desc>", %"class.StructuredBuffer<Mesh_desc>"* @"\01?Mesh_data_srv@@3V?$StructuredBuffer@UMesh_desc@@@@A"
  %4 = load %"class.StructuredBuffer<Instance_data>", %"class.StructuredBuffer<Instance_data>"* @"\01?Instance_data_srv@@3V?$StructuredBuffer@UInstance_data@@@@A"
  %5 = alloca [3 x float]
  %6 = alloca [3 x float]
  %7 = alloca [3 x float]
  %8 = alloca [3 x float]
  %9 = call i32 @dx.op.instanceIndex.i32(i32 142)  ; InstanceIndex()
  %10 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0)
  %11 = call %dx.types.Handle @"dx.op.createHandleForLib.class.StructuredBuffer<Instance_data>"(i32 160, %"class.StructuredBuffer<Instance_data>" %4)  ; CreateHandleForLib(Resource)
  %12 = call %dx.types.ResRet.i32 @dx.op.rawBufferLoad.i32(i32 139, %dx.types.Handle %11, i32 %9, i32 0, i8 1, i32 4)  ; RawBufferLoad(srv,index,elementOffset,mask,alignment)
  %13 = extractvalue %dx.types.ResRet.i32 %12, 0
  %14 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0)
  %15 = call %dx.types.Handle @"dx.op.createHandleForLib.class.StructuredBuffer<Mesh_desc>"(i32 160, %"class.StructuredBuffer<Mesh_desc>" %3)  ; CreateHandleForLib(Resource)
  %16 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0)
  %17 = call %dx.types.ResRet.i32 @dx.op.rawBufferLoad.i32(i32 139, %dx.types.Handle %15, i32 %13, i32 8, i8 1, i32 4)  ; RawBufferLoad(srv,index,elementOffset,mask,alignment)
  %18 = extractvalue %dx.types.ResRet.i32 %17, 0
  %19 = call %dx.types.ResRet.i32 @dx.op.rawBufferLoad.i32(i32 139, %dx.types.Handle %15, i32 %13, i32 12, i8 1, i32 4)  ; RawBufferLoad(srv,index,elementOffset,mask,alignment)
  %20 = extractvalue %dx.types.ResRet.i32 %19, 0
  %21 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0)
  %22 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0)
  %23 = call i32 @dx.op.primitiveIndex.i32(i32 161)  ; PrimitiveIndex()
  %24 = mul i32 %23, 3
  %25 = add i32 %24, %20
  %26 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0)
  %27 = call %dx.types.Handle @"dx.op.createHandleForLib.class.Buffer<unsigned int>"(i32 160, %"class.Buffer<unsigned int>" %1)  ; CreateHandleForLib(Resource)
  %28 = call %dx.types.ResRet.i32 @dx.op.bufferLoad.i32(i32 68, %dx.types.Handle %27, i32 %25, i32 undef)  ; BufferLoad(srv,index,wot)
  %29 = extractvalue %dx.types.ResRet.i32 %28, 0
  %30 = add i32 %25, 1
  %31 = call %dx.types.Handle @"dx.op.createHandleForLib.class.Buffer<unsigned int>"(i32 160, %"class.Buffer<unsigned int>" %1)  ; CreateHandleForLib(Resource)
  %32 = call %dx.types.ResRet.i32 @dx.op.bufferLoad.i32(i32 68, %dx.types.Handle %31, i32 %30, i32 undef)  ; BufferLoad(srv,index,wot)
  %33 = extractvalue %dx.types.ResRet.i32 %32, 0
  %34 = add i32 %25, 2
  %35 = call %dx.types.Handle @"dx.op.createHandleForLib.class.Buffer<unsigned int>"(i32 160, %"class.Buffer<unsigned int>" %1)  ; CreateHandleForLib(Resource)
  %36 = call %dx.types.ResRet.i32 @dx.op.bufferLoad.i32(i32 68, %dx.types.Handle %35, i32 %34, i32 undef)  ; BufferLoad(srv,index,wot)
  %37 = extractvalue %dx.types.ResRet.i32 %36, 0
  %38 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0)
  %39 = add i32 %29, %18
  %40 = add i32 %33, %18
  %41 = add i32 %37, %18
  %42 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0)
  %43 = getelementptr [3 x float], [3 x float]* %5, i32 0, i32 0
  %44 = getelementptr [3 x float], [3 x float]* %6, i32 0, i32 0
  %45 = getelementptr [3 x float], [3 x float]* %7, i32 0, i32 0
  %46 = getelementptr [3 x float], [3 x float]* %8, i32 0, i32 0
  %47 = call %dx.types.Handle @"dx.op.createHandleForLib.class.StructuredBuffer<Fat_vertex>"(i32 160, %"class.StructuredBuffer<Fat_vertex>" %2)  ; CreateHandleForLib(Resource)
  %48 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0)
  %49 = call %dx.types.ResRet.f32 @dx.op.rawBufferLoad.f32(i32 139, %dx.types.Handle %47, i32 %39, i32 12, i8 15, i32 4)  ; RawBufferLoad(srv,index,elementOffset,mask,alignment)
  %50 = extractvalue %dx.types.ResRet.f32 %49, 0
  %51 = extractvalue %dx.types.ResRet.f32 %49, 1
  %52 = extractvalue %dx.types.ResRet.f32 %49, 2
  %53 = extractvalue %dx.types.ResRet.f32 %49, 3
  store float %50, float* %43
  store float %51, float* %44
  store float %52, float* %45
  store float %53, float* %46
  %54 = getelementptr [3 x float], [3 x float]* %5, i32 0, i32 1
  %55 = getelementptr [3 x float], [3 x float]* %6, i32 0, i32 1
  %56 = getelementptr [3 x float], [3 x float]* %7, i32 0, i32 1
  %57 = getelementptr [3 x float], [3 x float]* %8, i32 0, i32 1
  %58 = call %dx.types.Handle @"dx.op.createHandleForLib.class.StructuredBuffer<Fat_vertex>"(i32 160, %"class.StructuredBuffer<Fat_vertex>" %2)  ; CreateHandleForLib(Resource)
  %59 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0)
  %60 = call %dx.types.ResRet.f32 @dx.op.rawBufferLoad.f32(i32 139, %dx.types.Handle %58, i32 %40, i32 12, i8 15, i32 4)  ; RawBufferLoad(srv,index,elementOffset,mask,alignment)
  %61 = extractvalue %dx.types.ResRet.f32 %60, 0
  %62 = extractvalue %dx.types.ResRet.f32 %60, 1
  %63 = extractvalue %dx.types.ResRet.f32 %60, 2
  %64 = extractvalue %dx.types.ResRet.f32 %60, 3
  store float %61, float* %54
  store float %62, float* %55
  store float %63, float* %56
  store float %64, float* %57
  %65 = getelementptr [3 x float], [3 x float]* %5, i32 0, i32 2
  %66 = getelementptr [3 x float], [3 x float]* %6, i32 0, i32 2
  %67 = getelementptr [3 x float], [3 x float]* %7, i32 0, i32 2
  %68 = getelementptr [3 x float], [3 x float]* %8, i32 0, i32 2
  %69 = call %dx.types.Handle @"dx.op.createHandleForLib.class.StructuredBuffer<Fat_vertex>"(i32 160, %"class.StructuredBuffer<Fat_vertex>" %2)  ; CreateHandleForLib(Resource)
  %70 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0)
  %71 = call %dx.types.ResRet.f32 @dx.op.rawBufferLoad.f32(i32 139, %dx.types.Handle %69, i32 %41, i32 12, i8 15, i32 4)  ; RawBufferLoad(srv,index,elementOffset,mask,alignment)
  %72 = extractvalue %dx.types.ResRet.f32 %71, 0
  %73 = extractvalue %dx.types.ResRet.f32 %71, 1
  %74 = extractvalue %dx.types.ResRet.f32 %71, 2
  %75 = extractvalue %dx.types.ResRet.f32 %71, 3
  store float %72, float* %65
  store float %73, float* %66
  store float %74, float* %67
  store float %75, float* %68
  %76 = getelementptr [3 x float], [3 x float]* %5, i32 0, i32 0
  %77 = getelementptr [3 x float], [3 x float]* %6, i32 0, i32 0
  %78 = getelementptr [3 x float], [3 x float]* %7, i32 0, i32 0
  %79 = getelementptr [3 x float], [3 x float]* %8, i32 0, i32 0
  %80 = load float, float* %76
  %81 = insertelement <4 x float> undef, float %80, i64 0
  %82 = load float, float* %77
  %83 = insertelement <4 x float> %81, float %82, i64 1
  %84 = load float, float* %78
  %85 = insertelement <4 x float> %83, float %84, i64 2
  %86 = load float, float* %79
  %87 = insertelement <4 x float> %85, float %86, i64 3
  %88 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0)
  %89 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0)
  %90 = getelementptr inbounds %struct.Payload_st, %struct.Payload_st* %payload, i32 0, i32 0
  store <4 x float> %87, <4 x float>* %90
  ret void
}

; Function Attrs: nounwind
define void @"\01?miss_entry@@YAXUPayload_st@@@Z"(%struct.Payload_st* noalias %payload) #0 {
  %1 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0)
  %2 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0)
  %3 = getelementptr inbounds %struct.Payload_st, %struct.Payload_st* %payload, i32 0, i32 0
  store <4 x float> <float 0.000000e+00, float 0.000000e+00, float 0.000000e+00, float 1.000000e+00>, <4 x float>* %3
  ret void
}

; Function Attrs: nounwind readnone
declare i32 @dx.op.dispatchRaysDimensions.i32(i32, i8) #1

; Function Attrs: nounwind readnone
declare i32 @dx.op.dispatchRaysIndex.i32(i32, i8) #1

; Function Attrs: nounwind readnone
declare float @dx.op.tertiary.f32(i32, float, float, float) #1

; Function Attrs: nounwind
declare void @dx.op.traceRay.struct.Payload_st(i32, %dx.types.Handle, i32, i32, i32, i32, i32, float, float, float, float, float, float, float, float, %struct.Payload_st*) #0

; Function Attrs: nounwind
declare void @dx.op.textureStore.f32(i32, %dx.types.Handle, i32, i32, i32, float, float, float, float, i8) #0

; Function Attrs: nounwind readnone
declare i32 @dx.op.primitiveIndex.i32(i32) #1

; Function Attrs: nounwind readnone
declare i32 @dx.op.instanceIndex.i32(i32) #1

; Function Attrs: nounwind readonly
declare %dx.types.ResRet.i32 @dx.op.rawBufferLoad.i32(i32, %dx.types.Handle, i32, i32, i8, i32) #2

; Function Attrs: nounwind readonly
declare %dx.types.ResRet.i32 @dx.op.bufferLoad.i32(i32, %dx.types.Handle, i32, i32) #2

; Function Attrs: nounwind readonly
declare %dx.types.ResRet.f32 @dx.op.rawBufferLoad.f32(i32, %dx.types.Handle, i32, i32, i8, i32) #2

; Function Attrs: nounwind readnone
declare float @dx.op.dot3.f32(i32, float, float, float, float, float, float) #1

; Function Attrs: nounwind readnone
declare float @dx.op.unary.f32(i32, float) #1

; Function Attrs: nounwind readonly
declare %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32, %dx.types.Handle, i32) #2

; Function Attrs: nounwind readonly
declare %dx.types.Handle @dx.op.createHandleForLib.Raygen_cb(i32, %Raygen_cb) #2

; Function Attrs: nounwind readonly
declare %dx.types.Handle @dx.op.createHandleForLib.struct.RaytracingAccelerationStructure(i32, %struct.RaytracingAccelerationStructure) #2

; Function Attrs: nounwind readonly
declare %dx.types.Handle @"dx.op.createHandleForLib.class.RWTexture2D<vector<float, 4> >"(i32, %"class.RWTexture2D<vector<float, 4> >") #2

; Function Attrs: nounwind readonly
declare %dx.types.Handle @"dx.op.createHandleForLib.class.StructuredBuffer<Instance_data>"(i32, %"class.StructuredBuffer<Instance_data>") #2

; Function Attrs: nounwind readonly
declare %dx.types.Handle @"dx.op.createHandleForLib.class.StructuredBuffer<Mesh_desc>"(i32, %"class.StructuredBuffer<Mesh_desc>") #2

; Function Attrs: nounwind readonly
declare %dx.types.Handle @"dx.op.createHandleForLib.class.Buffer<unsigned int>"(i32, %"class.Buffer<unsigned int>") #2

; Function Attrs: nounwind readonly
declare %dx.types.Handle @"dx.op.createHandleForLib.class.StructuredBuffer<Fat_vertex>"(i32, %"class.StructuredBuffer<Fat_vertex>") #2

; Function Attrs: nounwind readonly
declare %dx.types.Handle @dx.op.createHandleForLib.hostlayout.Camera_cb(i32, %hostlayout.Camera_cb) #2

attributes #0 = { nounwind }
attributes #1 = { nounwind readnone }
attributes #2 = { nounwind readonly }

!llvm.ident = !{!0}
!dx.version = !{!1}
!dx.valver = !{!1}
!dx.shaderModel = !{!2}
!dx.resources = !{!3}
!dx.typeAnnotations = !{!21}
!dx.entryPoints = !{!29, !32, !34, !36}

!0 = !{!"clang version 3.7 (tags/RELEASE_370/final)"}
!1 = !{i32 1, i32 5}
!2 = !{!"lib", i32 6, i32 5}
!3 = !{!4, !15, !18, null}
!4 = !{!5, !7, !9, !11, !13}
!5 = !{i32 0, %struct.RaytracingAccelerationStructure* @"\01?Scene_srv@@3URaytracingAccelerationStructure@@A", !"Scene_srv", i32 0, i32 0, i32 1, i32 16, i32 0, !6}
!6 = !{i32 0, i32 4}
!7 = !{i32 1, %"class.StructuredBuffer<Instance_data>"* @"\01?Instance_data_srv@@3V?$StructuredBuffer@UInstance_data@@@@A", !"Instance_data_srv", i32 0, i32 1, i32 1, i32 12, i32 0, !8}
!8 = !{i32 1, i32 4}
!9 = !{i32 2, %"class.StructuredBuffer<Mesh_desc>"* @"\01?Mesh_data_srv@@3V?$StructuredBuffer@UMesh_desc@@@@A", !"Mesh_data_srv", i32 0, i32 2, i32 1, i32 12, i32 0, !10}
!10 = !{i32 1, i32 16}
!11 = !{i32 3, %"class.StructuredBuffer<Fat_vertex>"* @"\01?Vertices_srv@@3V?$StructuredBuffer@UFat_vertex@@@@A", !"Vertices_srv", i32 0, i32 3, i32 1, i32 12, i32 0, !12}
!12 = !{i32 1, i32 40}
!13 = !{i32 4, %"class.Buffer<unsigned int>"* @"\01?Indices_srv@@3V?$Buffer@I@@A", !"Indices_srv", i32 0, i32 4, i32 1, i32 10, i32 0, !14}
!14 = !{i32 0, i32 5}
!15 = !{!16}
!16 = !{i32 0, %"class.RWTexture2D<vector<float, 4> >"* @"\01?Output_uav@@3V?$RWTexture2D@V?$vector@M$03@@@@A", !"Output_uav", i32 0, i32 0, i32 1, i32 2, i1 false, i1 false, i1 false, !17}
!17 = !{i32 0, i32 9}
!18 = !{!19, !20}
!19 = !{i32 0, %Raygen_cb* @Raygen_cb, !"Raygen_cb", i32 0, i32 0, i32 1, i32 32, null}
!20 = !{i32 1, %hostlayout.Camera_cb* @Camera_cb_legacy, !"Camera_cb", i32 0, i32 1, i32 1, i32 76, null}
!21 = !{i32 1, void ()* @"\01?raygen_entry@@YAXXZ", !22, void (%struct.Payload_st*, %struct.BuiltInTriangleIntersectionAttributes*)* @"\01?closethit_entry@@YAXUPayload_st@@UBuiltInTriangleIntersectionAttributes@@@Z", !25, void (%struct.Payload_st*)* @"\01?miss_entry@@YAXUPayload_st@@@Z", !28}
!22 = !{!23}
!23 = !{i32 1, !24, !24}
!24 = !{}
!25 = !{!23, !26, !27}
!26 = !{i32 2, !24, !24}
!27 = !{i32 0, !24, !24}
!28 = !{!23, !26}
!29 = !{null, !"", null, !3, !30}
!30 = !{i32 0, i64 65553, i32 5, !31}
!31 = !{i32 0}
!32 = !{void (%struct.Payload_st*, %struct.BuiltInTriangleIntersectionAttributes*)* @"\01?closethit_entry@@YAXUPayload_st@@UBuiltInTriangleIntersectionAttributes@@@Z", !"\01?closethit_entry@@YAXUPayload_st@@UBuiltInTriangleIntersectionAttributes@@@Z", null, null, !33}
!33 = !{i32 8, i32 10, i32 6, i32 16, i32 7, i32 8, i32 5, !31}
!34 = !{void (%struct.Payload_st*)* @"\01?miss_entry@@YAXUPayload_st@@@Z", !"\01?miss_entry@@YAXUPayload_st@@@Z", null, null, !35}
!35 = !{i32 8, i32 11, i32 6, i32 16, i32 5, !31}
!36 = !{void ()* @"\01?raygen_entry@@YAXXZ", !"\01?raygen_entry@@YAXXZ", null, null, !37}
!37 = !{i32 8, i32 7, i32 5, !31}
