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
; Colour                   0   xyz         1     NONE   float   xyz 
;
; shader hash: ded90e465f13c7965c077636c4e9221b
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
;
; Resource Bindings:
;
; Name                                 Type  Format         Dim      ID      HLSL Bind  Count
; ------------------------------ ---------- ------- ----------- ------- -------------- ------
;
;
; ViewId state:
;
; Number of inputs: 1, outputs: 7
; Outputs dependent on ViewId: {  }
; Inputs contributing to computation of Outputs:
;   output 0 depends on inputs: { 0 }
;   output 1 depends on inputs: { 0 }
;   output 4 depends on inputs: { 0 }
;   output 5 depends on inputs: { 0 }
;   output 6 depends on inputs: { 0 }
;
target datalayout = "e-m:e-p:32:32-i1:32-i8:32-i16:32-i32:32-i64:64-f16:32-f32:32-f64:64-n8:16:32:64"
target triple = "dxil-ms-dx"

@triangle_positions.v.1dim = internal constant [6 x float] [float 5.000000e-01, float -5.000000e-01, float 5.000000e-01, float 5.000000e-01, float -5.000000e-01, float 5.000000e-01], align 4
@triangle_colors.v.1dim = internal constant [9 x float] [float 1.000000e+00, float 0.000000e+00, float 0.000000e+00, float 0.000000e+00, float 1.000000e+00, float 0.000000e+00, float 0.000000e+00, float 0.000000e+00, float 1.000000e+00], align 4
@dx.nothing.a = internal constant [1 x i32] zeroinitializer

define void @main() {
  %1 = call i32 @dx.op.loadInput.i32(i32 4, i32 0, i32 0, i8 0, i32 undef)  ; LoadInput(inputSigId,rowIndex,colIndex,gsVertexAxis)
  %2 = mul i32 %1, 2
  %3 = add i32 0, %2
  %4 = getelementptr [6 x float], [6 x float]* @triangle_positions.v.1dim, i32 0, i32 %3
  %5 = load float, float* %4, align 4
  %6 = mul i32 %1, 2
  %7 = add i32 1, %6
  %8 = getelementptr [6 x float], [6 x float]* @triangle_positions.v.1dim, i32 0, i32 %7
  %9 = load float, float* %8, align 4
  %10 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0)
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 0, float %5)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 1, float %9)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 2, float 0.000000e+00)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 3, float 1.000000e+00)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  %11 = mul i32 %1, 3
  %12 = add i32 0, %11
  %13 = getelementptr [9 x float], [9 x float]* @triangle_colors.v.1dim, i32 0, i32 %12
  %14 = load float, float* %13, align 4
  %15 = mul i32 %1, 3
  %16 = add i32 1, %15
  %17 = getelementptr [9 x float], [9 x float]* @triangle_colors.v.1dim, i32 0, i32 %16
  %18 = load float, float* %17, align 4
  %19 = mul i32 %1, 3
  %20 = add i32 2, %19
  %21 = getelementptr [9 x float], [9 x float]* @triangle_colors.v.1dim, i32 0, i32 %20
  %22 = load float, float* %21, align 4
  %23 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0)
  call void @dx.op.storeOutput.f32(i32 5, i32 1, i32 0, i8 0, float %14)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 1, i32 0, i8 1, float %18)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 1, i32 0, i8 2, float %22)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  %24 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0)
  ret void
}

; Function Attrs: nounwind readnone
declare i32 @dx.op.loadInput.i32(i32, i32, i32, i8, i32) #0

; Function Attrs: nounwind
declare void @dx.op.storeOutput.f32(i32, i32, i32, i8, float) #1

attributes #0 = { nounwind readnone }
attributes #1 = { nounwind }

!llvm.ident = !{!0}
!dx.version = !{!1}
!dx.valver = !{!2}
!dx.shaderModel = !{!3}
!dx.viewIdState = !{!4}
!dx.entryPoints = !{!5}

!0 = !{!"clang version 3.7 (tags/RELEASE_370/final)"}
!1 = !{i32 1, i32 0}
!2 = !{i32 1, i32 5}
!3 = !{!"vs", i32 6, i32 0}
!4 = !{[3 x i32] [i32 1, i32 7, i32 115]}
!5 = !{void ()* @main, !"main", !6, null, !16}
!6 = !{!7, !11, null}
!7 = !{!8}
!8 = !{i32 0, !"SV_VertexID", i8 5, i8 1, !9, i8 0, i32 1, i8 1, i32 0, i8 0, !10}
!9 = !{i32 0}
!10 = !{i32 3, i32 1}
!11 = !{!12, !14}
!12 = !{i32 0, !"SV_Position", i8 9, i8 3, !9, i8 4, i32 1, i8 4, i32 0, i8 0, !13}
!13 = !{i32 3, i32 15}
!14 = !{i32 1, !"Colour", i8 9, i8 0, !9, i8 2, i32 1, i8 3, i32 1, i8 0, !15}
!15 = !{i32 3, i32 7}
!16 = !{i32 0, i64 1}
