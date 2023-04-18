;
; Input signature:
;
; Name                 Index   Mask Register SysValue  Format   Used
; -------------------- ----- ------ -------- -------- ------- ------
; Position                 0   xyzw        0     NONE   float   xyz 
; Colour                   0   xyzw        1     NONE   float   xyz 
;
;
; Output signature:
;
; Name                 Index   Mask Register SysValue  Format   Used
; -------------------- ----- ------ -------- -------- ------- ------
; SV_Position              0   xyzw        0      POS   float   xyzw
; Colour                   0   xyzw        1     NONE   float   xyzw
; Texcoord                 0   xyzw        2     NONE   float       
; Texcoord                 1   xyzw        3     NONE   float   xyzw
;
; shader hash: d38e7630fdcc152479e05076d0b573c2
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
; Position                 0                              
; Colour                   0                              
;
; Output signature:
;
; Name                 Index             InterpMode DynIdx
; -------------------- ----- ---------------------- ------
; SV_Position              0          noperspective       
; Colour                   0                 linear       
; Texcoord                 0                 linear       
; Texcoord                 1                 linear       
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
; cbuffer Camera_cb
; {
;
;   struct hostlayout.Camera_cb
;   {
;
;       row_major float4x4 View;                      ; Offset:    0
;       row_major float4x4 Projection;                ; Offset:   64
;   
;   } Camera_cb;                                      ; Offset:    0 Size:   128
;
; }
;
; cbuffer Object_cb
; {
;
;   struct hostlayout.Object_cb
;   {
;
;       row_major float4x4 World;                     ; Offset:    0
;   
;   } Object_cb;                                      ; Offset:    0 Size:    64
;
; }
;
;
; Resource Bindings:
;
; Name                                 Type  Format         Dim      ID      HLSL Bind  Count
; ------------------------------ ---------- ------- ----------- ------- -------------- ------
; Light_cb                          cbuffer      NA          NA     CB0            cb0     1
; Camera_cb                         cbuffer      NA          NA     CB1            cb1     1
; Object_cb                         cbuffer      NA          NA     CB2            cb2     1
;
;
; ViewId state:
;
; Number of inputs: 8, outputs: 16
; Outputs dependent on ViewId: {  }
; Inputs contributing to computation of Outputs:
;   output 0 depends on inputs: { 0, 1, 2 }
;   output 1 depends on inputs: { 0, 1, 2 }
;   output 2 depends on inputs: { 0, 1, 2 }
;   output 3 depends on inputs: { 0, 1, 2 }
;   output 4 depends on inputs: { 4 }
;   output 5 depends on inputs: { 5 }
;   output 6 depends on inputs: { 6 }
;   output 12 depends on inputs: { 0, 1, 2 }
;   output 13 depends on inputs: { 0, 1, 2 }
;   output 14 depends on inputs: { 0, 1, 2 }
;   output 15 depends on inputs: { 0, 1, 2 }
;
target datalayout = "e-m:e-p:32:32-i1:32-i8:32-i16:32-i32:32-i64:64-f16:32-f32:32-f64:64-n8:16:32:64"
target triple = "dxil-ms-dx"

%dx.types.Handle = type { i8* }
%dx.types.CBufRet.f32 = type { float, float, float, float }
%hostlayout.Light_cb = type { [4 x <4 x float>], [4 x <4 x float>], i32 }
%hostlayout.Camera_cb = type { [4 x <4 x float>], [4 x <4 x float>] }
%hostlayout.Object_cb = type { [4 x <4 x float>] }

@dx.nothing.a = internal constant [1 x i32] zeroinitializer

define void @main() {
  %1 = call %dx.types.Handle @dx.op.createHandle(i32 57, i8 2, i32 2, i32 2, i1 false)  ; CreateHandle(resourceClass,rangeId,index,nonUniformIndex)
  %2 = call %dx.types.Handle @dx.op.createHandle(i32 57, i8 2, i32 1, i32 1, i1 false)  ; CreateHandle(resourceClass,rangeId,index,nonUniformIndex)
  %3 = call %dx.types.Handle @dx.op.createHandle(i32 57, i8 2, i32 0, i32 0, i1 false)  ; CreateHandle(resourceClass,rangeId,index,nonUniformIndex)
  %4 = call float @dx.op.loadInput.f32(i32 4, i32 1, i32 0, i8 0, i32 undef)  ; LoadInput(inputSigId,rowIndex,colIndex,gsVertexAxis)
  %5 = call float @dx.op.loadInput.f32(i32 4, i32 1, i32 0, i8 1, i32 undef)  ; LoadInput(inputSigId,rowIndex,colIndex,gsVertexAxis)
  %6 = call float @dx.op.loadInput.f32(i32 4, i32 1, i32 0, i8 2, i32 undef)  ; LoadInput(inputSigId,rowIndex,colIndex,gsVertexAxis)
  %7 = call float @dx.op.loadInput.f32(i32 4, i32 0, i32 0, i8 0, i32 undef)  ; LoadInput(inputSigId,rowIndex,colIndex,gsVertexAxis)
  %8 = call float @dx.op.loadInput.f32(i32 4, i32 0, i32 0, i8 1, i32 undef)  ; LoadInput(inputSigId,rowIndex,colIndex,gsVertexAxis)
  %9 = call float @dx.op.loadInput.f32(i32 4, i32 0, i32 0, i8 2, i32 undef)  ; LoadInput(inputSigId,rowIndex,colIndex,gsVertexAxis)
  %10 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0)
  %11 = call %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32 59, %dx.types.Handle %1, i32 0)  ; CBufferLoadLegacy(handle,regIndex)
  %12 = extractvalue %dx.types.CBufRet.f32 %11, 0
  %13 = extractvalue %dx.types.CBufRet.f32 %11, 1
  %14 = extractvalue %dx.types.CBufRet.f32 %11, 2
  %15 = extractvalue %dx.types.CBufRet.f32 %11, 3
  %16 = call %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32 59, %dx.types.Handle %1, i32 1)  ; CBufferLoadLegacy(handle,regIndex)
  %17 = extractvalue %dx.types.CBufRet.f32 %16, 0
  %18 = extractvalue %dx.types.CBufRet.f32 %16, 1
  %19 = extractvalue %dx.types.CBufRet.f32 %16, 2
  %20 = extractvalue %dx.types.CBufRet.f32 %16, 3
  %21 = call %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32 59, %dx.types.Handle %1, i32 2)  ; CBufferLoadLegacy(handle,regIndex)
  %22 = extractvalue %dx.types.CBufRet.f32 %21, 0
  %23 = extractvalue %dx.types.CBufRet.f32 %21, 1
  %24 = extractvalue %dx.types.CBufRet.f32 %21, 2
  %25 = extractvalue %dx.types.CBufRet.f32 %21, 3
  %26 = call %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32 59, %dx.types.Handle %1, i32 3)  ; CBufferLoadLegacy(handle,regIndex)
  %27 = extractvalue %dx.types.CBufRet.f32 %26, 0
  %28 = extractvalue %dx.types.CBufRet.f32 %26, 1
  %29 = extractvalue %dx.types.CBufRet.f32 %26, 2
  %30 = extractvalue %dx.types.CBufRet.f32 %26, 3
  %31 = fmul fast float %7, %12
  %32 = call float @dx.op.tertiary.f32(i32 46, float %8, float %17, float %31)  ; FMad(a,b,c)
  %33 = call float @dx.op.tertiary.f32(i32 46, float %9, float %22, float %32)  ; FMad(a,b,c)
  %34 = call float @dx.op.tertiary.f32(i32 46, float 1.000000e+00, float %27, float %33)  ; FMad(a,b,c)
  %35 = fmul fast float %7, %13
  %36 = call float @dx.op.tertiary.f32(i32 46, float %8, float %18, float %35)  ; FMad(a,b,c)
  %37 = call float @dx.op.tertiary.f32(i32 46, float %9, float %23, float %36)  ; FMad(a,b,c)
  %38 = call float @dx.op.tertiary.f32(i32 46, float 1.000000e+00, float %28, float %37)  ; FMad(a,b,c)
  %39 = fmul fast float %7, %14
  %40 = call float @dx.op.tertiary.f32(i32 46, float %8, float %19, float %39)  ; FMad(a,b,c)
  %41 = call float @dx.op.tertiary.f32(i32 46, float %9, float %24, float %40)  ; FMad(a,b,c)
  %42 = call float @dx.op.tertiary.f32(i32 46, float 1.000000e+00, float %29, float %41)  ; FMad(a,b,c)
  %43 = fmul fast float %7, %15
  %44 = call float @dx.op.tertiary.f32(i32 46, float %8, float %20, float %43)  ; FMad(a,b,c)
  %45 = call float @dx.op.tertiary.f32(i32 46, float %9, float %25, float %44)  ; FMad(a,b,c)
  %46 = call float @dx.op.tertiary.f32(i32 46, float 1.000000e+00, float %30, float %45)  ; FMad(a,b,c)
  %47 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0)
  %48 = call %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32 59, %dx.types.Handle %2, i32 0)  ; CBufferLoadLegacy(handle,regIndex)
  %49 = extractvalue %dx.types.CBufRet.f32 %48, 0
  %50 = extractvalue %dx.types.CBufRet.f32 %48, 1
  %51 = extractvalue %dx.types.CBufRet.f32 %48, 2
  %52 = extractvalue %dx.types.CBufRet.f32 %48, 3
  %53 = call %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32 59, %dx.types.Handle %2, i32 1)  ; CBufferLoadLegacy(handle,regIndex)
  %54 = extractvalue %dx.types.CBufRet.f32 %53, 0
  %55 = extractvalue %dx.types.CBufRet.f32 %53, 1
  %56 = extractvalue %dx.types.CBufRet.f32 %53, 2
  %57 = extractvalue %dx.types.CBufRet.f32 %53, 3
  %58 = call %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32 59, %dx.types.Handle %2, i32 2)  ; CBufferLoadLegacy(handle,regIndex)
  %59 = extractvalue %dx.types.CBufRet.f32 %58, 0
  %60 = extractvalue %dx.types.CBufRet.f32 %58, 1
  %61 = extractvalue %dx.types.CBufRet.f32 %58, 2
  %62 = extractvalue %dx.types.CBufRet.f32 %58, 3
  %63 = call %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32 59, %dx.types.Handle %2, i32 3)  ; CBufferLoadLegacy(handle,regIndex)
  %64 = extractvalue %dx.types.CBufRet.f32 %63, 0
  %65 = extractvalue %dx.types.CBufRet.f32 %63, 1
  %66 = extractvalue %dx.types.CBufRet.f32 %63, 2
  %67 = extractvalue %dx.types.CBufRet.f32 %63, 3
  %68 = fmul fast float %34, %49
  %69 = call float @dx.op.tertiary.f32(i32 46, float %38, float %54, float %68)  ; FMad(a,b,c)
  %70 = call float @dx.op.tertiary.f32(i32 46, float %42, float %59, float %69)  ; FMad(a,b,c)
  %71 = call float @dx.op.tertiary.f32(i32 46, float %46, float %64, float %70)  ; FMad(a,b,c)
  %72 = fmul fast float %34, %50
  %73 = call float @dx.op.tertiary.f32(i32 46, float %38, float %55, float %72)  ; FMad(a,b,c)
  %74 = call float @dx.op.tertiary.f32(i32 46, float %42, float %60, float %73)  ; FMad(a,b,c)
  %75 = call float @dx.op.tertiary.f32(i32 46, float %46, float %65, float %74)  ; FMad(a,b,c)
  %76 = fmul fast float %34, %51
  %77 = call float @dx.op.tertiary.f32(i32 46, float %38, float %56, float %76)  ; FMad(a,b,c)
  %78 = call float @dx.op.tertiary.f32(i32 46, float %42, float %61, float %77)  ; FMad(a,b,c)
  %79 = call float @dx.op.tertiary.f32(i32 46, float %46, float %66, float %78)  ; FMad(a,b,c)
  %80 = fmul fast float %34, %52
  %81 = call float @dx.op.tertiary.f32(i32 46, float %38, float %57, float %80)  ; FMad(a,b,c)
  %82 = call float @dx.op.tertiary.f32(i32 46, float %42, float %62, float %81)  ; FMad(a,b,c)
  %83 = call float @dx.op.tertiary.f32(i32 46, float %46, float %67, float %82)  ; FMad(a,b,c)
  %84 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0)
  %85 = call %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32 59, %dx.types.Handle %2, i32 4)  ; CBufferLoadLegacy(handle,regIndex)
  %86 = extractvalue %dx.types.CBufRet.f32 %85, 0
  %87 = extractvalue %dx.types.CBufRet.f32 %85, 1
  %88 = extractvalue %dx.types.CBufRet.f32 %85, 2
  %89 = extractvalue %dx.types.CBufRet.f32 %85, 3
  %90 = call %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32 59, %dx.types.Handle %2, i32 5)  ; CBufferLoadLegacy(handle,regIndex)
  %91 = extractvalue %dx.types.CBufRet.f32 %90, 0
  %92 = extractvalue %dx.types.CBufRet.f32 %90, 1
  %93 = extractvalue %dx.types.CBufRet.f32 %90, 2
  %94 = extractvalue %dx.types.CBufRet.f32 %90, 3
  %95 = call %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32 59, %dx.types.Handle %2, i32 6)  ; CBufferLoadLegacy(handle,regIndex)
  %96 = extractvalue %dx.types.CBufRet.f32 %95, 0
  %97 = extractvalue %dx.types.CBufRet.f32 %95, 1
  %98 = extractvalue %dx.types.CBufRet.f32 %95, 2
  %99 = extractvalue %dx.types.CBufRet.f32 %95, 3
  %100 = call %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32 59, %dx.types.Handle %2, i32 7)  ; CBufferLoadLegacy(handle,regIndex)
  %101 = extractvalue %dx.types.CBufRet.f32 %100, 0
  %102 = extractvalue %dx.types.CBufRet.f32 %100, 1
  %103 = extractvalue %dx.types.CBufRet.f32 %100, 2
  %104 = extractvalue %dx.types.CBufRet.f32 %100, 3
  %105 = fmul fast float %71, %86
  %106 = call float @dx.op.tertiary.f32(i32 46, float %75, float %91, float %105)  ; FMad(a,b,c)
  %107 = call float @dx.op.tertiary.f32(i32 46, float %79, float %96, float %106)  ; FMad(a,b,c)
  %108 = call float @dx.op.tertiary.f32(i32 46, float %83, float %101, float %107)  ; FMad(a,b,c)
  %109 = fmul fast float %71, %87
  %110 = call float @dx.op.tertiary.f32(i32 46, float %75, float %92, float %109)  ; FMad(a,b,c)
  %111 = call float @dx.op.tertiary.f32(i32 46, float %79, float %97, float %110)  ; FMad(a,b,c)
  %112 = call float @dx.op.tertiary.f32(i32 46, float %83, float %102, float %111)  ; FMad(a,b,c)
  %113 = fmul fast float %71, %88
  %114 = call float @dx.op.tertiary.f32(i32 46, float %75, float %93, float %113)  ; FMad(a,b,c)
  %115 = call float @dx.op.tertiary.f32(i32 46, float %79, float %98, float %114)  ; FMad(a,b,c)
  %116 = call float @dx.op.tertiary.f32(i32 46, float %83, float %103, float %115)  ; FMad(a,b,c)
  %117 = fmul fast float %71, %89
  %118 = call float @dx.op.tertiary.f32(i32 46, float %75, float %94, float %117)  ; FMad(a,b,c)
  %119 = call float @dx.op.tertiary.f32(i32 46, float %79, float %99, float %118)  ; FMad(a,b,c)
  %120 = call float @dx.op.tertiary.f32(i32 46, float %83, float %104, float %119)  ; FMad(a,b,c)
  %121 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0)
  %122 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0)
  %123 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0)
  %124 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0)
  %125 = call %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32 59, %dx.types.Handle %1, i32 0)  ; CBufferLoadLegacy(handle,regIndex)
  %126 = extractvalue %dx.types.CBufRet.f32 %125, 0
  %127 = extractvalue %dx.types.CBufRet.f32 %125, 1
  %128 = extractvalue %dx.types.CBufRet.f32 %125, 2
  %129 = extractvalue %dx.types.CBufRet.f32 %125, 3
  %130 = call %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32 59, %dx.types.Handle %1, i32 1)  ; CBufferLoadLegacy(handle,regIndex)
  %131 = extractvalue %dx.types.CBufRet.f32 %130, 0
  %132 = extractvalue %dx.types.CBufRet.f32 %130, 1
  %133 = extractvalue %dx.types.CBufRet.f32 %130, 2
  %134 = extractvalue %dx.types.CBufRet.f32 %130, 3
  %135 = call %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32 59, %dx.types.Handle %1, i32 2)  ; CBufferLoadLegacy(handle,regIndex)
  %136 = extractvalue %dx.types.CBufRet.f32 %135, 0
  %137 = extractvalue %dx.types.CBufRet.f32 %135, 1
  %138 = extractvalue %dx.types.CBufRet.f32 %135, 2
  %139 = extractvalue %dx.types.CBufRet.f32 %135, 3
  %140 = call %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32 59, %dx.types.Handle %1, i32 3)  ; CBufferLoadLegacy(handle,regIndex)
  %141 = extractvalue %dx.types.CBufRet.f32 %140, 0
  %142 = extractvalue %dx.types.CBufRet.f32 %140, 1
  %143 = extractvalue %dx.types.CBufRet.f32 %140, 2
  %144 = extractvalue %dx.types.CBufRet.f32 %140, 3
  %145 = fmul fast float %7, %126
  %146 = call float @dx.op.tertiary.f32(i32 46, float %8, float %131, float %145)  ; FMad(a,b,c)
  %147 = call float @dx.op.tertiary.f32(i32 46, float %9, float %136, float %146)  ; FMad(a,b,c)
  %148 = call float @dx.op.tertiary.f32(i32 46, float 1.000000e+00, float %141, float %147)  ; FMad(a,b,c)
  %149 = fmul fast float %7, %127
  %150 = call float @dx.op.tertiary.f32(i32 46, float %8, float %132, float %149)  ; FMad(a,b,c)
  %151 = call float @dx.op.tertiary.f32(i32 46, float %9, float %137, float %150)  ; FMad(a,b,c)
  %152 = call float @dx.op.tertiary.f32(i32 46, float 1.000000e+00, float %142, float %151)  ; FMad(a,b,c)
  %153 = fmul fast float %7, %128
  %154 = call float @dx.op.tertiary.f32(i32 46, float %8, float %133, float %153)  ; FMad(a,b,c)
  %155 = call float @dx.op.tertiary.f32(i32 46, float %9, float %138, float %154)  ; FMad(a,b,c)
  %156 = call float @dx.op.tertiary.f32(i32 46, float 1.000000e+00, float %143, float %155)  ; FMad(a,b,c)
  %157 = fmul fast float %7, %129
  %158 = call float @dx.op.tertiary.f32(i32 46, float %8, float %134, float %157)  ; FMad(a,b,c)
  %159 = call float @dx.op.tertiary.f32(i32 46, float %9, float %139, float %158)  ; FMad(a,b,c)
  %160 = call float @dx.op.tertiary.f32(i32 46, float 1.000000e+00, float %144, float %159)  ; FMad(a,b,c)
  %161 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0)
  %162 = call %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32 59, %dx.types.Handle %3, i32 0)  ; CBufferLoadLegacy(handle,regIndex)
  %163 = extractvalue %dx.types.CBufRet.f32 %162, 0
  %164 = extractvalue %dx.types.CBufRet.f32 %162, 1
  %165 = extractvalue %dx.types.CBufRet.f32 %162, 2
  %166 = extractvalue %dx.types.CBufRet.f32 %162, 3
  %167 = call %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32 59, %dx.types.Handle %3, i32 1)  ; CBufferLoadLegacy(handle,regIndex)
  %168 = extractvalue %dx.types.CBufRet.f32 %167, 0
  %169 = extractvalue %dx.types.CBufRet.f32 %167, 1
  %170 = extractvalue %dx.types.CBufRet.f32 %167, 2
  %171 = extractvalue %dx.types.CBufRet.f32 %167, 3
  %172 = call %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32 59, %dx.types.Handle %3, i32 2)  ; CBufferLoadLegacy(handle,regIndex)
  %173 = extractvalue %dx.types.CBufRet.f32 %172, 0
  %174 = extractvalue %dx.types.CBufRet.f32 %172, 1
  %175 = extractvalue %dx.types.CBufRet.f32 %172, 2
  %176 = extractvalue %dx.types.CBufRet.f32 %172, 3
  %177 = call %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32 59, %dx.types.Handle %3, i32 3)  ; CBufferLoadLegacy(handle,regIndex)
  %178 = extractvalue %dx.types.CBufRet.f32 %177, 0
  %179 = extractvalue %dx.types.CBufRet.f32 %177, 1
  %180 = extractvalue %dx.types.CBufRet.f32 %177, 2
  %181 = extractvalue %dx.types.CBufRet.f32 %177, 3
  %182 = fmul fast float %148, %163
  %183 = call float @dx.op.tertiary.f32(i32 46, float %152, float %168, float %182)  ; FMad(a,b,c)
  %184 = call float @dx.op.tertiary.f32(i32 46, float %156, float %173, float %183)  ; FMad(a,b,c)
  %185 = call float @dx.op.tertiary.f32(i32 46, float %160, float %178, float %184)  ; FMad(a,b,c)
  %186 = fmul fast float %148, %164
  %187 = call float @dx.op.tertiary.f32(i32 46, float %152, float %169, float %186)  ; FMad(a,b,c)
  %188 = call float @dx.op.tertiary.f32(i32 46, float %156, float %174, float %187)  ; FMad(a,b,c)
  %189 = call float @dx.op.tertiary.f32(i32 46, float %160, float %179, float %188)  ; FMad(a,b,c)
  %190 = fmul fast float %148, %165
  %191 = call float @dx.op.tertiary.f32(i32 46, float %152, float %170, float %190)  ; FMad(a,b,c)
  %192 = call float @dx.op.tertiary.f32(i32 46, float %156, float %175, float %191)  ; FMad(a,b,c)
  %193 = call float @dx.op.tertiary.f32(i32 46, float %160, float %180, float %192)  ; FMad(a,b,c)
  %194 = fmul fast float %148, %166
  %195 = call float @dx.op.tertiary.f32(i32 46, float %152, float %171, float %194)  ; FMad(a,b,c)
  %196 = call float @dx.op.tertiary.f32(i32 46, float %156, float %176, float %195)  ; FMad(a,b,c)
  %197 = call float @dx.op.tertiary.f32(i32 46, float %160, float %181, float %196)  ; FMad(a,b,c)
  %198 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0)
  %199 = call %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32 59, %dx.types.Handle %3, i32 4)  ; CBufferLoadLegacy(handle,regIndex)
  %200 = extractvalue %dx.types.CBufRet.f32 %199, 0
  %201 = extractvalue %dx.types.CBufRet.f32 %199, 1
  %202 = extractvalue %dx.types.CBufRet.f32 %199, 2
  %203 = extractvalue %dx.types.CBufRet.f32 %199, 3
  %204 = call %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32 59, %dx.types.Handle %3, i32 5)  ; CBufferLoadLegacy(handle,regIndex)
  %205 = extractvalue %dx.types.CBufRet.f32 %204, 0
  %206 = extractvalue %dx.types.CBufRet.f32 %204, 1
  %207 = extractvalue %dx.types.CBufRet.f32 %204, 2
  %208 = extractvalue %dx.types.CBufRet.f32 %204, 3
  %209 = call %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32 59, %dx.types.Handle %3, i32 6)  ; CBufferLoadLegacy(handle,regIndex)
  %210 = extractvalue %dx.types.CBufRet.f32 %209, 0
  %211 = extractvalue %dx.types.CBufRet.f32 %209, 1
  %212 = extractvalue %dx.types.CBufRet.f32 %209, 2
  %213 = extractvalue %dx.types.CBufRet.f32 %209, 3
  %214 = call %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32 59, %dx.types.Handle %3, i32 7)  ; CBufferLoadLegacy(handle,regIndex)
  %215 = extractvalue %dx.types.CBufRet.f32 %214, 0
  %216 = extractvalue %dx.types.CBufRet.f32 %214, 1
  %217 = extractvalue %dx.types.CBufRet.f32 %214, 2
  %218 = extractvalue %dx.types.CBufRet.f32 %214, 3
  %219 = fmul fast float %185, %200
  %220 = call float @dx.op.tertiary.f32(i32 46, float %189, float %205, float %219)  ; FMad(a,b,c)
  %221 = call float @dx.op.tertiary.f32(i32 46, float %193, float %210, float %220)  ; FMad(a,b,c)
  %222 = call float @dx.op.tertiary.f32(i32 46, float %197, float %215, float %221)  ; FMad(a,b,c)
  %223 = fmul fast float %185, %201
  %224 = call float @dx.op.tertiary.f32(i32 46, float %189, float %206, float %223)  ; FMad(a,b,c)
  %225 = call float @dx.op.tertiary.f32(i32 46, float %193, float %211, float %224)  ; FMad(a,b,c)
  %226 = call float @dx.op.tertiary.f32(i32 46, float %197, float %216, float %225)  ; FMad(a,b,c)
  %227 = fmul fast float %185, %202
  %228 = call float @dx.op.tertiary.f32(i32 46, float %189, float %207, float %227)  ; FMad(a,b,c)
  %229 = call float @dx.op.tertiary.f32(i32 46, float %193, float %212, float %228)  ; FMad(a,b,c)
  %230 = call float @dx.op.tertiary.f32(i32 46, float %197, float %217, float %229)  ; FMad(a,b,c)
  %231 = fmul fast float %185, %203
  %232 = call float @dx.op.tertiary.f32(i32 46, float %189, float %208, float %231)  ; FMad(a,b,c)
  %233 = call float @dx.op.tertiary.f32(i32 46, float %193, float %213, float %232)  ; FMad(a,b,c)
  %234 = call float @dx.op.tertiary.f32(i32 46, float %197, float %218, float %233)  ; FMad(a,b,c)
  %235 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0)
  %236 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0)
  %237 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0)
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 0, float %108)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 1, float %112)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 2, float %116)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 3, float %120)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 1, i32 0, i8 0, float %4)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 1, i32 0, i8 1, float %5)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 1, i32 0, i8 2, float %6)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 1, i32 0, i8 3, float 1.000000e+00)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 3, i32 0, i8 0, float %222)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 3, i32 0, i8 1, float %226)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 3, i32 0, i8 2, float %230)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 3, i32 0, i8 3, float %234)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  %238 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0)
  ret void
}

; Function Attrs: nounwind readnone
declare float @dx.op.loadInput.f32(i32, i32, i32, i8, i32) #0

; Function Attrs: nounwind
declare void @dx.op.storeOutput.f32(i32, i32, i32, i8, float) #1

; Function Attrs: nounwind readonly
declare %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32, %dx.types.Handle, i32) #2

; Function Attrs: nounwind readnone
declare float @dx.op.tertiary.f32(i32, float, float, float) #0

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
!dx.viewIdState = !{!9}
!dx.entryPoints = !{!10}

!0 = !{!"dxcoob 1.7.2212.40 (e043f4a12)"}
!1 = !{i32 1, i32 0}
!2 = !{i32 1, i32 7}
!3 = !{!"vs", i32 6, i32 0}
!4 = !{null, null, !5, null}
!5 = !{!6, !7, !8}
!6 = !{i32 0, %hostlayout.Light_cb* undef, !"", i32 0, i32 0, i32 1, i32 132, null}
!7 = !{i32 1, %hostlayout.Camera_cb* undef, !"", i32 0, i32 1, i32 1, i32 128, null}
!8 = !{i32 2, %hostlayout.Object_cb* undef, !"", i32 0, i32 2, i32 1, i32 64, null}
!9 = !{[10 x i32] [i32 8, i32 16, i32 61455, i32 61455, i32 61455, i32 0, i32 16, i32 32, i32 64, i32 0]}
!10 = !{void ()* @main, !"main", !11, !4, !24}
!11 = !{!12, !17, null}
!12 = !{!13, !16}
!13 = !{i32 0, !"Position", i8 9, i8 0, !14, i8 0, i32 1, i8 4, i32 0, i8 0, !15}
!14 = !{i32 0}
!15 = !{i32 3, i32 7}
!16 = !{i32 1, !"Colour", i8 9, i8 0, !14, i8 0, i32 1, i8 4, i32 1, i8 0, !15}
!17 = !{!18, !20, !21, !22}
!18 = !{i32 0, !"SV_Position", i8 9, i8 3, !14, i8 4, i32 1, i8 4, i32 0, i8 0, !19}
!19 = !{i32 3, i32 15}
!20 = !{i32 1, !"Colour", i8 9, i8 0, !14, i8 2, i32 1, i8 4, i32 1, i8 0, !19}
!21 = !{i32 2, !"Texcoord", i8 9, i8 0, !14, i8 2, i32 1, i8 4, i32 2, i8 0, null}
!22 = !{i32 3, !"Texcoord", i8 9, i8 0, !23, i8 2, i32 1, i8 4, i32 3, i8 0, !19}
!23 = !{i32 1}
!24 = !{i32 0, i64 1}
