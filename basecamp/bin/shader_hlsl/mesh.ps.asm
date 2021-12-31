;
; Input signature:
;
; Name                 Index   Mask Register SysValue  Format   Used
; -------------------- ----- ------ -------- -------- ------- ------
; SV_Position              0   xyzw        0      POS   float       
; Colour                   0   xyzw        1     NONE   float   xyz 
; Texcoord                 0   xyzw        2     NONE   float       
; Texcoord                 1   xyzw        3     NONE   float   xyzw
;
;
; Output signature:
;
; Name                 Index   Mask Register SysValue  Format   Used
; -------------------- ----- ------ -------- -------- ------- ------
; SV_Target                0   xyzw        0   TARGET   float   xyzw
;
; shader hash: 04ee85789e006b890459c63b7912e997
;
; Pipeline Runtime Information: 
;
; Pixel Shader
; DepthOutput=0
; SampleFrequency=0
;
;
; Input signature:
;
; Name                 Index             InterpMode DynIdx
; -------------------- ----- ---------------------- ------
; SV_Position              0          noperspective       
; Colour                   0                 linear       
; Texcoord                 0                 linear       
; Texcoord                 1                 linear       
;
; Output signature:
;
; Name                 Index             InterpMode DynIdx
; -------------------- ----- ---------------------- ------
; SV_Target                0                              
;
; Buffer Definitions:
;
; cbuffer Light_cb
; {
;
;   struct hostlayout.Light_cb
;   {
;
;       row_major float4x4 Light_view;                ; Offset:    0
;       row_major float4x4 Light_projection;          ; Offset:   64
;       int Receive_shadow;                           ; Offset:  128
;   
;   } Light_cb;                                       ; Offset:    0 Size:   132
;
; }
;
;
; Resource Bindings:
;
; Name                                 Type  Format         Dim      ID      HLSL Bind  Count
; ------------------------------ ---------- ------- ----------- ------- -------------- ------
; Light_cb                          cbuffer      NA          NA     CB0            cb0     1
; Point_sampler                     sampler      NA          NA      S0             s0     1
; Shadow_map_srv                    texture     f32          2d      T0             t0     1
;
;
; ViewId state:
;
; Number of inputs: 16, outputs: 4
; Outputs dependent on ViewId: {  }
; Inputs contributing to computation of Outputs:
;   output 0 depends on inputs: { 4, 12, 13, 14, 15 }
;   output 1 depends on inputs: { 5, 12, 13, 14, 15 }
;   output 2 depends on inputs: { 6, 12, 13, 14, 15 }
;
target datalayout = "e-m:e-p:32:32-i1:32-i8:32-i16:32-i32:32-i64:64-f16:32-f32:32-f64:64-n8:16:32:64"
target triple = "dxil-ms-dx"

%dx.types.Handle = type { i8* }
%dx.types.CBufRet.i32 = type { i32, i32, i32, i32 }
%dx.types.ResRet.f32 = type { float, float, float, float, i32 }
%"class.Texture2D<vector<float, 4> >" = type { <4 x float>, %"class.Texture2D<vector<float, 4> >::mips_type" }
%"class.Texture2D<vector<float, 4> >::mips_type" = type { i32 }
%hostlayout.Light_cb = type { [4 x <4 x float>], [4 x <4 x float>], i32 }
%struct.SamplerState = type { i32 }

@dx.nothing.a = internal constant [1 x i32] zeroinitializer

define void @main() {
  %1 = call %dx.types.Handle @dx.op.createHandle(i32 57, i8 0, i32 0, i32 0, i1 false)  ; CreateHandle(resourceClass,rangeId,index,nonUniformIndex)
  %2 = call %dx.types.Handle @dx.op.createHandle(i32 57, i8 3, i32 0, i32 0, i1 false)  ; CreateHandle(resourceClass,rangeId,index,nonUniformIndex)
  %3 = call %dx.types.Handle @dx.op.createHandle(i32 57, i8 2, i32 0, i32 0, i1 false)  ; CreateHandle(resourceClass,rangeId,index,nonUniformIndex)
  %4 = call float @dx.op.loadInput.f32(i32 4, i32 3, i32 0, i8 0, i32 undef)  ; LoadInput(inputSigId,rowIndex,colIndex,gsVertexAxis)
  %5 = call float @dx.op.loadInput.f32(i32 4, i32 3, i32 0, i8 1, i32 undef)  ; LoadInput(inputSigId,rowIndex,colIndex,gsVertexAxis)
  %6 = call float @dx.op.loadInput.f32(i32 4, i32 3, i32 0, i8 2, i32 undef)  ; LoadInput(inputSigId,rowIndex,colIndex,gsVertexAxis)
  %7 = call float @dx.op.loadInput.f32(i32 4, i32 3, i32 0, i8 3, i32 undef)  ; LoadInput(inputSigId,rowIndex,colIndex,gsVertexAxis)
  %8 = call float @dx.op.loadInput.f32(i32 4, i32 1, i32 0, i8 0, i32 undef)  ; LoadInput(inputSigId,rowIndex,colIndex,gsVertexAxis)
  %9 = call float @dx.op.loadInput.f32(i32 4, i32 1, i32 0, i8 1, i32 undef)  ; LoadInput(inputSigId,rowIndex,colIndex,gsVertexAxis)
  %10 = call float @dx.op.loadInput.f32(i32 4, i32 1, i32 0, i8 2, i32 undef)  ; LoadInput(inputSigId,rowIndex,colIndex,gsVertexAxis)
  %11 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0)
  %12 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0)
  %13 = call %dx.types.CBufRet.i32 @dx.op.cbufferLoadLegacy.i32(i32 59, %dx.types.Handle %3, i32 8)  ; CBufferLoadLegacy(handle,regIndex)
  %14 = extractvalue %dx.types.CBufRet.i32 %13, 0
  %15 = icmp sgt i32 %14, 0
  %16 = icmp ne i1 %15, false
  %17 = icmp ne i1 %16, false
  br i1 %17, label %18, label %60

; <label>:18                                      ; preds = %0
  %19 = fdiv fast float %4, %7
  %20 = fdiv fast float %5, %7
  %21 = fdiv fast float %6, %7
  %22 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0)
  %23 = fcmp fast olt float %19, -1.000000e+00
  %24 = icmp ne i1 %23, false
  %25 = fcmp fast ogt float %19, 1.000000e+00
  %26 = icmp ne i1 %25, false
  %27 = or i1 %24, %26
  %28 = fcmp fast olt float %20, -1.000000e+00
  %29 = icmp ne i1 %28, false
  %30 = or i1 %27, %29
  %31 = fcmp fast ogt float %20, 1.000000e+00
  %32 = icmp ne i1 %31, false
  %33 = or i1 %30, %32
  %34 = fcmp fast olt float %21, 0.000000e+00
  %35 = icmp ne i1 %34, false
  %36 = or i1 %33, %35
  %37 = fcmp fast ogt float %21, 1.000000e+00
  %38 = icmp ne i1 %37, false
  %39 = or i1 %36, %38
  %40 = icmp ne i1 %39, false
  br i1 %40, label %41, label %43

; <label>:41                                      ; preds = %18
  %42 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0)
  br label %58

; <label>:43                                      ; preds = %18
  %44 = fmul fast float %19, 5.000000e-01
  %45 = fadd fast float %44, 5.000000e-01
  %46 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0)
  %47 = fmul fast float %20, -5.000000e-01
  %48 = fadd fast float %47, 5.000000e-01
  %49 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0)
  %50 = call %dx.types.ResRet.f32 @dx.op.sample.f32(i32 60, %dx.types.Handle %1, %dx.types.Handle %2, float %45, float %48, float undef, float undef, i32 0, i32 0, i32 undef, float undef)  ; Sample(srv,sampler,coord0,coord1,coord2,coord3,offset0,offset1,offset2,clamp)
  %51 = extractvalue %dx.types.ResRet.f32 %50, 0
  %52 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0)
  %53 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0)
  %54 = fcmp fast olt float %21, %51
  %55 = icmp ne i1 %54, false
  %56 = select i1 %55, float 1.000000e+00, float 0x3FB99999A0000000
  %57 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0)
  br label %58

; <label>:58                                      ; preds = %43, %41
  %59 = phi float [ 1.000000e+00, %41 ], [ %56, %43 ]
  br label %60

; <label>:60                                      ; preds = %58, %0
  %61 = phi float [ %59, %58 ], [ 1.000000e+00, %0 ]
  %62 = fmul fast float 1.000000e+00, %8
  %63 = fmul fast float 1.000000e+00, %9
  %64 = fmul fast float 1.000000e+00, %10
  %65 = fmul fast float %62, %61
  %66 = fmul fast float %63, %61
  %67 = fmul fast float %64, %61
  %68 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0)
  %69 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0)
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 0, float %65)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 1, float %66)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 2, float %67)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 3, float 1.000000e+00)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  ret void
}

; Function Attrs: nounwind readnone
declare float @dx.op.loadInput.f32(i32, i32, i32, i8, i32) #0

; Function Attrs: nounwind
declare void @dx.op.storeOutput.f32(i32, i32, i32, i8, float) #1

; Function Attrs: nounwind readonly
declare %dx.types.ResRet.f32 @dx.op.sample.f32(i32, %dx.types.Handle, %dx.types.Handle, float, float, float, float, i32, i32, i32, float) #2

; Function Attrs: nounwind readonly
declare %dx.types.CBufRet.i32 @dx.op.cbufferLoadLegacy.i32(i32, %dx.types.Handle, i32) #2

; Function Attrs: nounwind readonly
declare %dx.types.Handle @dx.op.createHandle(i32, i8, i32, i32, i1) #2

attributes #0 = { nounwind readnone }
attributes #1 = { nounwind }
attributes #2 = { nounwind readonly }

!llvm.ident = !{!0}
!dx.version = !{!1}
!dx.valver = !{!2}
!dx.shaderModel = !{!3}
!dx.resources = !{!4}
!dx.viewIdState = !{!12}
!dx.entryPoints = !{!13}

!0 = !{!"clang version 3.7 (tags/RELEASE_370/final)"}
!1 = !{i32 1, i32 0}
!2 = !{i32 1, i32 5}
!3 = !{!"ps", i32 6, i32 0}
!4 = !{!5, null, !8, !10}
!5 = !{!6}
!6 = !{i32 0, %"class.Texture2D<vector<float, 4> >"* undef, !"", i32 0, i32 0, i32 1, i32 2, i32 0, !7}
!7 = !{i32 0, i32 9}
!8 = !{!9}
!9 = !{i32 0, %hostlayout.Light_cb* undef, !"", i32 0, i32 0, i32 1, i32 132, null}
!10 = !{!11}
!11 = !{i32 0, %struct.SamplerState* undef, !"", i32 0, i32 0, i32 1, i32 0, null}
!12 = !{[18 x i32] [i32 16, i32 4, i32 0, i32 0, i32 0, i32 0, i32 1, i32 2, i32 4, i32 0, i32 0, i32 0, i32 0, i32 0, i32 7, i32 7, i32 7, i32 7]}
!13 = !{void ()* @main, !"main", !14, !4, !26}
!14 = !{!15, !24, null}
!15 = !{!16, !18, !20, !21}
!16 = !{i32 0, !"SV_Position", i8 9, i8 3, !17, i8 4, i32 1, i8 4, i32 0, i8 0, null}
!17 = !{i32 0}
!18 = !{i32 1, !"Colour", i8 9, i8 0, !17, i8 2, i32 1, i8 4, i32 1, i8 0, !19}
!19 = !{i32 3, i32 7}
!20 = !{i32 2, !"Texcoord", i8 9, i8 0, !17, i8 2, i32 1, i8 4, i32 2, i8 0, null}
!21 = !{i32 3, !"Texcoord", i8 9, i8 0, !22, i8 2, i32 1, i8 4, i32 3, i8 0, !23}
!22 = !{i32 1}
!23 = !{i32 3, i32 15}
!24 = !{!25}
!25 = !{i32 0, !"SV_Target", i8 9, i8 16, !17, i8 0, i32 1, i8 4, i32 0, i8 0, !23}
!26 = !{i32 0, i64 1}
