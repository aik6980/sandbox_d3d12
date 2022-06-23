;
; Note: shader requires additional functionality:
;       UAVs at every shader stage
;
; shader hash: 921aa186a9415115f00fd4145f820dd9
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
;
; Resource Bindings:
;
; Name                                 Type  Format         Dim      ID      HLSL Bind  Count
; ------------------------------ ---------- ------- ----------- ------- -------------- ------
; Raygen_cb                         cbuffer      NA          NA     CB0            cb0     1
; Scene_srv                         texture     i32         ras      T0             t0     1
; Output_uav                            UAV     f32          2d      U0             u0     1
;
target datalayout = "e-m:e-p:32:32-i1:32-i8:32-i16:32-i32:32-i64:64-f16:32-f32:32-f64:64-n8:16:32:64"
target triple = "dxil-ms-dx"

%struct.RaytracingAccelerationStructure = type { i32 }
%"class.RWTexture2D<vector<float, 4> >" = type { <4 x float> }
%Raygen_cb = type { <4 x float>, <4 x float> }
%struct.RayDesc = type { <3 x float>, float, <3 x float>, float }
%struct.Payload_st = type { <4 x float> }
%dx.types.Handle = type { i8* }
%dx.types.CBufRet.f32 = type { float, float, float, float }
%struct.BuiltInTriangleIntersectionAttributes = type { <2 x float> }

@"\01?Scene_srv@@3URaytracingAccelerationStructure@@A" = external constant %struct.RaytracingAccelerationStructure, align 4
@"\01?Output_uav@@3V?$RWTexture2D@V?$vector@M$03@@@@A" = external constant %"class.RWTexture2D<vector<float, 4> >", align 4
@Raygen_cb = external constant %Raygen_cb
@dx.nothing.a = internal constant [1 x i32] zeroinitializer

; Function Attrs: nounwind
define void @"\01?raygen_entry@@YAXXZ"() #0 {
  %1 = load %struct.RaytracingAccelerationStructure, %struct.RaytracingAccelerationStructure* @"\01?Scene_srv@@3URaytracingAccelerationStructure@@A"
  %2 = load %"class.RWTexture2D<vector<float, 4> >", %"class.RWTexture2D<vector<float, 4> >"* @"\01?Output_uav@@3V?$RWTexture2D@V?$vector@M$03@@@@A"
  %3 = load %Raygen_cb, %Raygen_cb* @Raygen_cb
  %4 = alloca %struct.RayDesc, align 4
  %5 = alloca %struct.Payload_st, align 4
  %6 = call %dx.types.Handle @dx.op.createHandleForLib.Raygen_cb(i32 160, %Raygen_cb %3)  ; CreateHandleForLib(Resource)
  %7 = call i32 @dx.op.dispatchRaysIndex.i32(i32 145, i8 0)  ; DispatchRaysIndex(col)
  %8 = call i32 @dx.op.dispatchRaysIndex.i32(i32 145, i8 1)  ; DispatchRaysIndex(col)
  %9 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0)
  %10 = uitofp i32 %7 to float
  %11 = uitofp i32 %8 to float
  %12 = call i32 @dx.op.dispatchRaysDimensions.i32(i32 146, i8 0)  ; DispatchRaysDimensions(col)
  %13 = call i32 @dx.op.dispatchRaysDimensions.i32(i32 146, i8 1)  ; DispatchRaysDimensions(col)
  %14 = uitofp i32 %12 to float
  %15 = uitofp i32 %13 to float
  %16 = fdiv fast float %10, %14
  %17 = fdiv fast float %11, %15
  %18 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0)
  %19 = call %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32 59, %dx.types.Handle %6, i32 0)  ; CBufferLoadLegacy(handle,regIndex)
  %20 = extractvalue %dx.types.CBufRet.f32 %19, 0
  %21 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0)
  %22 = call %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32 59, %dx.types.Handle %6, i32 0)  ; CBufferLoadLegacy(handle,regIndex)
  %23 = extractvalue %dx.types.CBufRet.f32 %22, 1
  %24 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0)
  %25 = call %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32 59, %dx.types.Handle %6, i32 0)  ; CBufferLoadLegacy(handle,regIndex)
  %26 = extractvalue %dx.types.CBufRet.f32 %25, 2
  %27 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0)
  %28 = call %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32 59, %dx.types.Handle %6, i32 0)  ; CBufferLoadLegacy(handle,regIndex)
  %29 = extractvalue %dx.types.CBufRet.f32 %28, 3
  %30 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0)
  %31 = fsub fast float %26, %20
  %32 = fmul fast float %16, %31
  %33 = fadd fast float %20, %32
  %34 = fsub fast float %29, %23
  %35 = fmul fast float %17, %34
  %36 = fadd fast float %23, %35
  %37 = insertelement <3 x float> undef, float %33, i64 0
  %38 = insertelement <3 x float> %37, float %36, i64 1
  %39 = insertelement <3 x float> %38, float 0.000000e+00, i64 2
  %40 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0)
  %41 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0)
  %42 = call %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32 59, %dx.types.Handle %6, i32 1)  ; CBufferLoadLegacy(handle,regIndex)
  %43 = extractvalue %dx.types.CBufRet.f32 %42, 0
  %44 = extractvalue %dx.types.CBufRet.f32 %42, 1
  %45 = extractvalue %dx.types.CBufRet.f32 %42, 2
  %46 = extractvalue %dx.types.CBufRet.f32 %42, 3
  %47 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0)
  %48 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0)
  %49 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0)
  %50 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0)
  %51 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0)
  %52 = fcmp fast oge float %33, %43
  %53 = fcmp fast ole float %33, %45
  %54 = and i1 %52, %53
  %55 = fcmp fast oge float %36, %44
  %56 = fcmp fast ole float %36, %46
  %57 = and i1 %55, %56
  %58 = and i1 %54, %57
  %59 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0)
  br i1 %58, label %60, label %98

; <label>:60                                      ; preds = %0
  %61 = getelementptr inbounds %struct.RayDesc, %struct.RayDesc* %4, i32 0, i32 0
  %62 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0)
  store <3 x float> %39, <3 x float>* %61, align 4
  %63 = getelementptr inbounds %struct.RayDesc, %struct.RayDesc* %4, i32 0, i32 2
  %64 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0)
  store <3 x float> <float 0.000000e+00, float 0.000000e+00, float 1.000000e+00>, <3 x float>* %63, align 4
  %65 = getelementptr inbounds %struct.RayDesc, %struct.RayDesc* %4, i32 0, i32 1
  %66 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0)
  store float 0x3F50624DE0000000, float* %65, align 4
  %67 = getelementptr inbounds %struct.RayDesc, %struct.RayDesc* %4, i32 0, i32 3
  %68 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0)
  store float 1.000000e+04, float* %67, align 4
  %69 = getelementptr inbounds %struct.Payload_st, %struct.Payload_st* %5, i32 0, i32 0
  %70 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0)
  store <4 x float> zeroinitializer, <4 x float>* %69, align 4
  %71 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0)
  %72 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0)
  %73 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0)
  %74 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0)
  %75 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0)
  %76 = call %dx.types.Handle @dx.op.createHandleForLib.struct.RaytracingAccelerationStructure(i32 160, %struct.RaytracingAccelerationStructure %1)  ; CreateHandleForLib(Resource)
  %77 = getelementptr %struct.RayDesc, %struct.RayDesc* %4, i32 0, i32 0
  %78 = load <3 x float>, <3 x float>* %77
  %79 = extractelement <3 x float> %78, i64 0
  %80 = extractelement <3 x float> %78, i64 1
  %81 = extractelement <3 x float> %78, i64 2
  %82 = getelementptr %struct.RayDesc, %struct.RayDesc* %4, i32 0, i32 1
  %83 = load float, float* %82
  %84 = getelementptr %struct.RayDesc, %struct.RayDesc* %4, i32 0, i32 2
  %85 = load <3 x float>, <3 x float>* %84
  %86 = extractelement <3 x float> %85, i64 0
  %87 = extractelement <3 x float> %85, i64 1
  %88 = extractelement <3 x float> %85, i64 2
  %89 = getelementptr %struct.RayDesc, %struct.RayDesc* %4, i32 0, i32 3
  %90 = load float, float* %89
  call void @dx.op.traceRay.struct.Payload_st(i32 157, %dx.types.Handle %76, i32 16, i32 -1, i32 0, i32 1, i32 0, float %79, float %80, float %81, float %83, float %86, float %87, float %88, float %90, %struct.Payload_st* %5)  ; TraceRay(AccelerationStructure,RayFlags,InstanceInclusionMask,RayContributionToHitGroupIndex,MultiplierForGeometryContributionToShaderIndex,MissShaderIndex,Origin_X,Origin_Y,Origin_Z,TMin,Direction_X,Direction_Y,Direction_Z,TMax,payload)
  %91 = getelementptr inbounds %struct.Payload_st, %struct.Payload_st* %5, i32 0, i32 0
  %92 = load <4 x float>, <4 x float>* %91, align 4
  %93 = call %dx.types.Handle @"dx.op.createHandleForLib.class.RWTexture2D<vector<float, 4> >"(i32 160, %"class.RWTexture2D<vector<float, 4> >" %2)  ; CreateHandleForLib(Resource)
  %94 = extractelement <4 x float> %92, i64 0
  %95 = extractelement <4 x float> %92, i64 1
  %96 = extractelement <4 x float> %92, i64 2
  %97 = extractelement <4 x float> %92, i64 3
  call void @dx.op.textureStore.f32(i32 67, %dx.types.Handle %93, i32 %7, i32 %8, i32 undef, float %94, float %95, float %96, float %97, i8 15)  ; TextureStore(srv,coord0,coord1,coord2,value0,value1,value2,value3,mask)
  br label %101

; <label>:98                                      ; preds = %0
  %99 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0)
  %100 = call %dx.types.Handle @"dx.op.createHandleForLib.class.RWTexture2D<vector<float, 4> >"(i32 160, %"class.RWTexture2D<vector<float, 4> >" %2)  ; CreateHandleForLib(Resource)
  call void @dx.op.textureStore.f32(i32 67, %dx.types.Handle %100, i32 %7, i32 %8, i32 undef, float %16, float %17, float 0.000000e+00, float 1.000000e+00, i8 15)  ; TextureStore(srv,coord0,coord1,coord2,value0,value1,value2,value3,mask)
  br label %101

; <label>:101                                     ; preds = %98, %60
  %102 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0)
  ret void
}

; Function Attrs: nounwind
define void @"\01?closethit_entry@@YAXUPayload_st@@UBuiltInTriangleIntersectionAttributes@@@Z"(%struct.Payload_st* noalias %payload, %struct.BuiltInTriangleIntersectionAttributes* %attr) #0 {
  %1 = getelementptr inbounds %struct.BuiltInTriangleIntersectionAttributes, %struct.BuiltInTriangleIntersectionAttributes* %attr, i32 0, i32 0
  %2 = load <2 x float>, <2 x float>* %1, align 4
  %3 = extractelement <2 x float> %2, i32 0
  %4 = fsub fast float 1.000000e+00, %3
  %5 = getelementptr inbounds %struct.BuiltInTriangleIntersectionAttributes, %struct.BuiltInTriangleIntersectionAttributes* %attr, i32 0, i32 0
  %6 = load <2 x float>, <2 x float>* %5, align 4
  %7 = extractelement <2 x float> %6, i32 1
  %8 = fsub fast float %4, %7
  %9 = getelementptr inbounds %struct.BuiltInTriangleIntersectionAttributes, %struct.BuiltInTriangleIntersectionAttributes* %attr, i32 0, i32 0
  %10 = load <2 x float>, <2 x float>* %9, align 4
  %11 = extractelement <2 x float> %10, i32 0
  %12 = getelementptr inbounds %struct.BuiltInTriangleIntersectionAttributes, %struct.BuiltInTriangleIntersectionAttributes* %attr, i32 0, i32 0
  %13 = load <2 x float>, <2 x float>* %12, align 4
  %14 = extractelement <2 x float> %13, i32 1
  %15 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0)
  %16 = insertelement <4 x float> undef, float %8, i64 0
  %17 = insertelement <4 x float> %16, float %11, i64 1
  %18 = insertelement <4 x float> %17, float %14, i64 2
  %19 = insertelement <4 x float> %18, float 1.000000e+00, i64 3
  %20 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0)
  %21 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0)
  %22 = getelementptr inbounds %struct.Payload_st, %struct.Payload_st* %payload, i32 0, i32 0
  store <4 x float> %19, <4 x float>* %22
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

; Function Attrs: nounwind
declare void @dx.op.traceRay.struct.Payload_st(i32, %dx.types.Handle, i32, i32, i32, i32, i32, float, float, float, float, float, float, float, float, %struct.Payload_st*) #0

; Function Attrs: nounwind
declare void @dx.op.textureStore.f32(i32, %dx.types.Handle, i32, i32, i32, float, float, float, float, i8) #0

; Function Attrs: nounwind readonly
declare %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32, %dx.types.Handle, i32) #2

; Function Attrs: nounwind readonly
declare %dx.types.Handle @dx.op.createHandleForLib.Raygen_cb(i32, %Raygen_cb) #2

; Function Attrs: nounwind readonly
declare %dx.types.Handle @dx.op.createHandleForLib.struct.RaytracingAccelerationStructure(i32, %struct.RaytracingAccelerationStructure) #2

; Function Attrs: nounwind readonly
declare %dx.types.Handle @"dx.op.createHandleForLib.class.RWTexture2D<vector<float, 4> >"(i32, %"class.RWTexture2D<vector<float, 4> >") #2

attributes #0 = { nounwind }
attributes #1 = { nounwind readnone }
attributes #2 = { nounwind readonly }

!llvm.ident = !{!0}
!dx.version = !{!1}
!dx.valver = !{!1}
!dx.shaderModel = !{!2}
!dx.resources = !{!3}
!dx.typeAnnotations = !{!12}
!dx.entryPoints = !{!20, !23, !25, !27}

!0 = !{!"clang version 3.7 (tags/RELEASE_370/final)"}
!1 = !{i32 1, i32 5}
!2 = !{!"lib", i32 6, i32 5}
!3 = !{!4, !7, !10, null}
!4 = !{!5}
!5 = !{i32 0, %struct.RaytracingAccelerationStructure* @"\01?Scene_srv@@3URaytracingAccelerationStructure@@A", !"Scene_srv", i32 0, i32 0, i32 1, i32 16, i32 0, !6}
!6 = !{i32 0, i32 4}
!7 = !{!8}
!8 = !{i32 0, %"class.RWTexture2D<vector<float, 4> >"* @"\01?Output_uav@@3V?$RWTexture2D@V?$vector@M$03@@@@A", !"Output_uav", i32 0, i32 0, i32 1, i32 2, i1 false, i1 false, i1 false, !9}
!9 = !{i32 0, i32 9}
!10 = !{!11}
!11 = !{i32 0, %Raygen_cb* @Raygen_cb, !"Raygen_cb", i32 0, i32 0, i32 1, i32 32, null}
!12 = !{i32 1, void ()* @"\01?raygen_entry@@YAXXZ", !13, void (%struct.Payload_st*, %struct.BuiltInTriangleIntersectionAttributes*)* @"\01?closethit_entry@@YAXUPayload_st@@UBuiltInTriangleIntersectionAttributes@@@Z", !16, void (%struct.Payload_st*)* @"\01?miss_entry@@YAXUPayload_st@@@Z", !19}
!13 = !{!14}
!14 = !{i32 1, !15, !15}
!15 = !{}
!16 = !{!14, !17, !18}
!17 = !{i32 2, !15, !15}
!18 = !{i32 0, !15, !15}
!19 = !{!14, !17}
!20 = !{null, !"", null, !3, !21}
!21 = !{i32 0, i64 65537, i32 5, !22}
!22 = !{i32 0}
!23 = !{void (%struct.Payload_st*, %struct.BuiltInTriangleIntersectionAttributes*)* @"\01?closethit_entry@@YAXUPayload_st@@UBuiltInTriangleIntersectionAttributes@@@Z", !"\01?closethit_entry@@YAXUPayload_st@@UBuiltInTriangleIntersectionAttributes@@@Z", null, null, !24}
!24 = !{i32 8, i32 10, i32 6, i32 16, i32 7, i32 8, i32 5, !22}
!25 = !{void (%struct.Payload_st*)* @"\01?miss_entry@@YAXUPayload_st@@@Z", !"\01?miss_entry@@YAXUPayload_st@@@Z", null, null, !26}
!26 = !{i32 8, i32 11, i32 6, i32 16, i32 5, !22}
!27 = !{void ()* @"\01?raygen_entry@@YAXXZ", !"\01?raygen_entry@@YAXXZ", null, null, !28}
!28 = !{i32 8, i32 7, i32 5, !22}
