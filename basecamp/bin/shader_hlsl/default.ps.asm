;
; Input signature:
;
; Name                 Index   Mask Register SysValue  Format   Used
; -------------------- ----- ------ -------- -------- ------- ------
; SV_Position              0   xyzw        0      POS   float       
; Colour                   0   xyzw        1     NONE   float       
;
;
; Output signature:
;
; Name                 Index   Mask Register SysValue  Format   Used
; -------------------- ----- ------ -------- -------- ------- ------
; SV_Target                0   xyzw        0   TARGET   float   xyzw
;
; shader hash: 2ca3c86c8f25c88e18070932ff1d5b16
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
;
; Output signature:
;
; Name                 Index             InterpMode DynIdx
; -------------------- ----- ---------------------- ------
; SV_Target                0                              
;
; Buffer Definitions:
;
; cbuffer key
; {
;
;   struct key
;   {
;
;       float key_i;                                  ; Offset:    0
;   
;   } key;                                            ; Offset:    0 Size:     4
;
; }
;
; cbuffer cb_ps
; {
;
;   struct cb_ps
;   {
;
;       float4 colour;                                ; Offset:    0
;       float4 colour1;                               ; Offset:   16
;   
;   } cb_ps;                                          ; Offset:    0 Size:    32
;
; }
;
;
; Resource Bindings:
;
; Name                                 Type  Format         Dim      ID      HLSL Bind  Count
; ------------------------------ ---------- ------- ----------- ------- -------------- ------
; key                               cbuffer      NA          NA     CB0            cb0     1
; cb_ps                             cbuffer      NA          NA     CB1            cb1     1
;
;
; ViewId state:
;
; Number of inputs: 8, outputs: 4
; Outputs dependent on ViewId: {  }
; Inputs contributing to computation of Outputs:
;
target datalayout = "e-m:e-p:32:32-i1:32-i8:32-i16:32-i32:32-i64:64-f16:32-f32:32-f64:64-n8:16:32:64"
target triple = "dxil-ms-dx"

%dx.types.Handle = type { i8* }
%dx.types.CBufRet.f32 = type { float, float, float, float }
%key = type { float }
%cb_ps = type { <4 x float>, <4 x float> }

@dx.nothing.a = internal constant [1 x i32] zeroinitializer

define void @main() {
  %1 = call %dx.types.Handle @dx.op.createHandle(i32 57, i8 2, i32 1, i32 1, i1 false)  ; CreateHandle(resourceClass,rangeId,index,nonUniformIndex)
  %2 = call %dx.types.Handle @dx.op.createHandle(i32 57, i8 2, i32 0, i32 0, i1 false)  ; CreateHandle(resourceClass,rangeId,index,nonUniformIndex)
  %3 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0)
  %4 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0)
  %5 = call %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32 59, %dx.types.Handle %2, i32 0)  ; CBufferLoadLegacy(handle,regIndex)
  %6 = extractvalue %dx.types.CBufRet.f32 %5, 0
  %7 = fcmp fast ogt float %6, 0.000000e+00
  %8 = icmp ne i1 %7, false
  %9 = icmp ne i1 %8, false
  br i1 %9, label %10, label %16

; <label>:10                                      ; preds = %0
  %11 = call %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32 59, %dx.types.Handle %1, i32 0)  ; CBufferLoadLegacy(handle,regIndex)
  %12 = extractvalue %dx.types.CBufRet.f32 %11, 0
  %13 = extractvalue %dx.types.CBufRet.f32 %11, 1
  %14 = extractvalue %dx.types.CBufRet.f32 %11, 2
  %15 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0)
  br label %16

; <label>:16                                      ; preds = %10, %0
  %17 = phi float [ %12, %10 ], [ 0.000000e+00, %0 ]
  %18 = phi float [ %13, %10 ], [ 1.000000e+00, %0 ]
  %19 = phi float [ %14, %10 ], [ 0.000000e+00, %0 ]
  %20 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0)
  %21 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0)
  %22 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0)
  %23 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0)
  %24 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0)
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 0, float %17)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 1, float %18)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 2, float %19)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 3, float 1.000000e+00)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  ret void
}

; Function Attrs: nounwind
declare void @dx.op.storeOutput.f32(i32, i32, i32, i8, float) #0

; Function Attrs: nounwind readonly
declare %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32, %dx.types.Handle, i32) #1

; Function Attrs: nounwind readonly
declare %dx.types.Handle @dx.op.createHandle(i32, i8, i32, i32, i1) #1

attributes #0 = { nounwind }
attributes #1 = { nounwind readonly }

!llvm.ident = !{!0}
!dx.version = !{!1}
!dx.valver = !{!2}
!dx.shaderModel = !{!3}
!dx.resources = !{!4}
!dx.viewIdState = !{!8}
!dx.entryPoints = !{!9}

!0 = !{!"dxcoob 1.7.2212.40 (e043f4a12)"}
!1 = !{i32 1, i32 0}
!2 = !{i32 1, i32 7}
!3 = !{!"ps", i32 6, i32 0}
!4 = !{null, null, !5, null}
!5 = !{!6, !7}
!6 = !{i32 0, %key* undef, !"", i32 0, i32 0, i32 1, i32 4, null}
!7 = !{i32 1, %cb_ps* undef, !"", i32 0, i32 1, i32 1, i32 32, null}
!8 = !{[10 x i32] [i32 8, i32 4, i32 0, i32 0, i32 0, i32 0, i32 0, i32 0, i32 0, i32 0]}
!9 = !{void ()* @main, !"main", !10, !4, !18}
!10 = !{!11, !15, null}
!11 = !{!12, !14}
!12 = !{i32 0, !"SV_Position", i8 9, i8 3, !13, i8 4, i32 1, i8 4, i32 0, i8 0, null}
!13 = !{i32 0}
!14 = !{i32 1, !"Colour", i8 9, i8 0, !13, i8 2, i32 1, i8 4, i32 1, i8 0, null}
!15 = !{!16}
!16 = !{i32 0, !"SV_Target", i8 9, i8 16, !13, i8 0, i32 1, i8 4, i32 0, i8 0, !17}
!17 = !{i32 3, i32 15}
!18 = !{i32 0, i64 1}
