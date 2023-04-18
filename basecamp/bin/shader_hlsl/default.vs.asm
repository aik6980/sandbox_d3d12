;
; Input signature:
;
; Name                 Index   Mask Register SysValue  Format   Used
; -------------------- ----- ------ -------- -------- ------- ------
; SV_VertexID              0   x           0   VERTID    uint   x   
;
;
; Output signature:
;
; Name                 Index   Mask Register SysValue  Format   Used
; -------------------- ----- ------ -------- -------- ------- ------
; SV_Position              0   xyzw        0      POS   float   xyzw
; Colour                   0   xyzw        1     NONE   float       
;
; shader hash: 6e9a1d8b78da1b194a1f898c37405fe9
;
; Pipeline Runtime Information: 
;
; Vertex Shader
; OutputPositionPresent=1
;
;
; Input signature:
;
; Name                 Index             InterpMode DynIdx
; -------------------- ----- ---------------------- ------
; SV_VertexID              0                              
;
; Output signature:
;
; Name                 Index             InterpMode DynIdx
; -------------------- ----- ---------------------- ------
; SV_Position              0          noperspective       
; Colour                   0                 linear       
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
; cbuffer cb_vs
; {
;
;   struct cb_vs
;   {
;
;       float2 offset;                                ; Offset:    0
;       float2 scale;                                 ; Offset:    8
;       float offsety;                                ; Offset:   16
;   
;   } cb_vs;                                          ; Offset:    0 Size:    20
;
; }
;
;
; Resource Bindings:
;
; Name                                 Type  Format         Dim      ID      HLSL Bind  Count
; ------------------------------ ---------- ------- ----------- ------- -------------- ------
; key                               cbuffer      NA          NA     CB0            cb0     1
; cb_vs                             cbuffer      NA          NA     CB1            cb1     1
;
;
; ViewId state:
;
; Number of inputs: 1, outputs: 8
; Outputs dependent on ViewId: {  }
; Inputs contributing to computation of Outputs:
;   output 0 depends on inputs: { 0 }
;   output 1 depends on inputs: { 0 }
;   output 2 depends on inputs: { 0 }
;
target datalayout = "e-m:e-p:32:32-i1:32-i8:32-i16:32-i32:32-i64:64-f16:32-f32:32-f64:64-n8:16:32:64"
target triple = "dxil-ms-dx"

%dx.types.Handle = type { i8* }
%dx.types.CBufRet.f32 = type { float, float, float, float }
%key = type { float }
%cb_vs = type { <2 x float>, <2 x float>, float }

@tri.v.1dim = internal constant [9 x float] [float 0.000000e+00, float 5.000000e-01, float 5.000000e-01, float 5.000000e-01, float -5.000000e-01, float 5.000000e-01, float -5.000000e-01, float -5.000000e-01, float 5.000000e-01], align 4
@dx.nothing.a = internal constant [1 x i32] zeroinitializer

define void @main() {
  %1 = call %dx.types.Handle @dx.op.createHandle(i32 57, i8 2, i32 1, i32 1, i1 false)  ; CreateHandle(resourceClass,rangeId,index,nonUniformIndex)
  %2 = call %dx.types.Handle @dx.op.createHandle(i32 57, i8 2, i32 0, i32 0, i1 false)  ; CreateHandle(resourceClass,rangeId,index,nonUniformIndex)
  %3 = call i32 @dx.op.loadInput.i32(i32 4, i32 0, i32 0, i8 0, i32 undef)  ; LoadInput(inputSigId,rowIndex,colIndex,gsVertexAxis)
  %4 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0)
  %5 = call %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32 59, %dx.types.Handle %2, i32 0)  ; CBufferLoadLegacy(handle,regIndex)
  %6 = extractvalue %dx.types.CBufRet.f32 %5, 0
  %7 = fcmp fast ogt float %6, 0.000000e+00
  %8 = icmp ne i1 %7, false
  %9 = icmp ne i1 %8, false
  br i1 %9, label %10, label %15

; <label>:10                                      ; preds = %0
  %11 = call %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32 59, %dx.types.Handle %1, i32 0)  ; CBufferLoadLegacy(handle,regIndex)
  %12 = extractvalue %dx.types.CBufRet.f32 %11, 0
  %13 = extractvalue %dx.types.CBufRet.f32 %11, 1
  %14 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0)
  br label %15

; <label>:15                                      ; preds = %10, %0
  %16 = phi float [ %12, %10 ], [ 0.000000e+00, %0 ]
  %17 = phi float [ %13, %10 ], [ 2.500000e-01, %0 ]
  %18 = mul i32 %3, 3
  %19 = add i32 0, %18
  %20 = getelementptr [9 x float], [9 x float]* @tri.v.1dim, i32 0, i32 %19
  %21 = load float, float* %20, align 4
  %22 = mul i32 %3, 3
  %23 = add i32 1, %22
  %24 = getelementptr [9 x float], [9 x float]* @tri.v.1dim, i32 0, i32 %23
  %25 = load float, float* %24, align 4
  %26 = call %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32 59, %dx.types.Handle %1, i32 0)  ; CBufferLoadLegacy(handle,regIndex)
  %27 = extractvalue %dx.types.CBufRet.f32 %26, 2
  %28 = extractvalue %dx.types.CBufRet.f32 %26, 3
  %29 = fmul fast float %21, %27
  %30 = fmul fast float %25, %28
  %31 = fadd fast float %29, %16
  %32 = fadd fast float %30, %17
  %33 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0)
  %34 = call %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32 59, %dx.types.Handle %1, i32 1)  ; CBufferLoadLegacy(handle,regIndex)
  %35 = extractvalue %dx.types.CBufRet.f32 %34, 0
  %36 = fadd fast float %32, %35
  %37 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0)
  %38 = mul i32 %3, 3
  %39 = add i32 2, %38
  %40 = getelementptr [9 x float], [9 x float]* @tri.v.1dim, i32 0, i32 %39
  %41 = load float, float* %40, align 4
  %42 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0)
  %43 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0)
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 0, float %31)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 1, float %36)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 2, float %41)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 3, float 1.000000e+00)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  %44 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0)
  ret void
}

; Function Attrs: nounwind readnone
declare i32 @dx.op.loadInput.i32(i32, i32, i32, i8, i32) #0

; Function Attrs: nounwind
declare void @dx.op.storeOutput.f32(i32, i32, i32, i8, float) #1

; Function Attrs: nounwind readonly
declare %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32, %dx.types.Handle, i32) #2

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
!dx.viewIdState = !{!8}
!dx.entryPoints = !{!9}

!0 = !{!"dxcoob 1.7.2212.40 (e043f4a12)"}
!1 = !{i32 1, i32 0}
!2 = !{i32 1, i32 7}
!3 = !{!"vs", i32 6, i32 0}
!4 = !{null, null, !5, null}
!5 = !{!6, !7}
!6 = !{i32 0, %key* undef, !"", i32 0, i32 0, i32 1, i32 4, null}
!7 = !{i32 1, %cb_vs* undef, !"", i32 0, i32 1, i32 1, i32 20, null}
!8 = !{[3 x i32] [i32 1, i32 8, i32 7]}
!9 = !{void ()* @main, !"main", !10, !4, !19}
!10 = !{!11, !15, null}
!11 = !{!12}
!12 = !{i32 0, !"SV_VertexID", i8 5, i8 1, !13, i8 0, i32 1, i8 1, i32 0, i8 0, !14}
!13 = !{i32 0}
!14 = !{i32 3, i32 1}
!15 = !{!16, !18}
!16 = !{i32 0, !"SV_Position", i8 9, i8 3, !13, i8 4, i32 1, i8 4, i32 0, i8 0, !17}
!17 = !{i32 3, i32 15}
!18 = !{i32 1, !"Colour", i8 9, i8 0, !13, i8 2, i32 1, i8 4, i32 1, i8 0, null}
!19 = !{i32 0, i64 1}
