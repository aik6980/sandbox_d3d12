;
; Input signature:
;
; Name                 Index   Mask Register SysValue  Format   Used
; -------------------- ----- ------ -------- -------- ------- ------
; Position                 0   xyz         0     NONE   float   xyz 
; Colour                   0   xyz         1     NONE   float   xyz 
; Instance_                0   xyzw        2     NONE   float   xyzw
; Instance_                1   xyzw        3     NONE   float       
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
; shader hash: e5abdb7af6e630770c400ada42261a35
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
; Instance_                0                              
; Instance_                1                              
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
; cbuffer Camera_cb_v51
; {
;
;   struct hostlayout.Camera_cb_v51
;   {
;
;       struct hostlayout.struct.Camera_st
;       {
;
;           row_major float4x4 View;                  ; Offset:    0
;           row_major float4x4 Projection;            ; Offset:   64
;       
;       } Camera_cb_v51;                              ; Offset:    0
;
;   
;   } Camera_cb_v51;                                  ; Offset:    0 Size:   128
;
; }
;
;
; Resource Bindings:
;
; Name                                 Type  Format         Dim      ID      HLSL Bind  Count
; ------------------------------ ---------- ------- ----------- ------- -------------- ------
; Light_cb                          cbuffer      NA          NA     CB0            cb0     1
; Camera_cb_v51                     cbuffer      NA          NA     CB1            cb1     1
;
;
; ViewId state:
;
; Number of inputs: 16, outputs: 16
; Outputs dependent on ViewId: {  }
; Inputs contributing to computation of Outputs:
;   output 0 depends on inputs: { 0, 1, 2, 8, 9, 10, 11 }
;   output 1 depends on inputs: { 0, 1, 2, 8, 9, 10, 11 }
;   output 2 depends on inputs: { 0, 1, 2, 8, 9, 10, 11 }
;   output 3 depends on inputs: { 0, 1, 2, 8, 9, 10, 11 }
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
%hostlayout.Camera_cb_v51 = type { %hostlayout.struct.Camera_st }
%hostlayout.struct.Camera_st = type { [4 x <4 x float>], [4 x <4 x float>] }

@dx.nothing.a = internal constant [1 x i32] zeroinitializer

define void @main() {
  %1 = call %dx.types.Handle @dx.op.createHandle(i32 57, i8 2, i32 1, i32 1, i1 false)  ; CreateHandle(resourceClass,rangeId,index,nonUniformIndex)
  %2 = call %dx.types.Handle @dx.op.createHandle(i32 57, i8 2, i32 0, i32 0, i1 false)  ; CreateHandle(resourceClass,rangeId,index,nonUniformIndex)
  %3 = call float @dx.op.loadInput.f32(i32 4, i32 2, i32 0, i8 0, i32 undef)  ; LoadInput(inputSigId,rowIndex,colIndex,gsVertexAxis)
  %4 = call float @dx.op.loadInput.f32(i32 4, i32 2, i32 0, i8 1, i32 undef)  ; LoadInput(inputSigId,rowIndex,colIndex,gsVertexAxis)
  %5 = call float @dx.op.loadInput.f32(i32 4, i32 2, i32 0, i8 2, i32 undef)  ; LoadInput(inputSigId,rowIndex,colIndex,gsVertexAxis)
  %6 = call float @dx.op.loadInput.f32(i32 4, i32 2, i32 0, i8 3, i32 undef)  ; LoadInput(inputSigId,rowIndex,colIndex,gsVertexAxis)
  %7 = call float @dx.op.loadInput.f32(i32 4, i32 1, i32 0, i8 0, i32 undef)  ; LoadInput(inputSigId,rowIndex,colIndex,gsVertexAxis)
  %8 = call float @dx.op.loadInput.f32(i32 4, i32 1, i32 0, i8 1, i32 undef)  ; LoadInput(inputSigId,rowIndex,colIndex,gsVertexAxis)
  %9 = call float @dx.op.loadInput.f32(i32 4, i32 1, i32 0, i8 2, i32 undef)  ; LoadInput(inputSigId,rowIndex,colIndex,gsVertexAxis)
  %10 = call float @dx.op.loadInput.f32(i32 4, i32 0, i32 0, i8 0, i32 undef)  ; LoadInput(inputSigId,rowIndex,colIndex,gsVertexAxis)
  %11 = call float @dx.op.loadInput.f32(i32 4, i32 0, i32 0, i8 1, i32 undef)  ; LoadInput(inputSigId,rowIndex,colIndex,gsVertexAxis)
  %12 = call float @dx.op.loadInput.f32(i32 4, i32 0, i32 0, i8 2, i32 undef)  ; LoadInput(inputSigId,rowIndex,colIndex,gsVertexAxis)
  %13 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0)
  %14 = fadd fast float %10, %3
  %15 = fadd fast float %11, %4
  %16 = fadd fast float %12, %5
  %17 = fadd fast float 1.000000e+00, %6
  %18 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0)
  %19 = call %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32 59, %dx.types.Handle %1, i32 0)  ; CBufferLoadLegacy(handle,regIndex)
  %20 = extractvalue %dx.types.CBufRet.f32 %19, 0
  %21 = extractvalue %dx.types.CBufRet.f32 %19, 1
  %22 = extractvalue %dx.types.CBufRet.f32 %19, 2
  %23 = extractvalue %dx.types.CBufRet.f32 %19, 3
  %24 = call %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32 59, %dx.types.Handle %1, i32 1)  ; CBufferLoadLegacy(handle,regIndex)
  %25 = extractvalue %dx.types.CBufRet.f32 %24, 0
  %26 = extractvalue %dx.types.CBufRet.f32 %24, 1
  %27 = extractvalue %dx.types.CBufRet.f32 %24, 2
  %28 = extractvalue %dx.types.CBufRet.f32 %24, 3
  %29 = call %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32 59, %dx.types.Handle %1, i32 2)  ; CBufferLoadLegacy(handle,regIndex)
  %30 = extractvalue %dx.types.CBufRet.f32 %29, 0
  %31 = extractvalue %dx.types.CBufRet.f32 %29, 1
  %32 = extractvalue %dx.types.CBufRet.f32 %29, 2
  %33 = extractvalue %dx.types.CBufRet.f32 %29, 3
  %34 = call %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32 59, %dx.types.Handle %1, i32 3)  ; CBufferLoadLegacy(handle,regIndex)
  %35 = extractvalue %dx.types.CBufRet.f32 %34, 0
  %36 = extractvalue %dx.types.CBufRet.f32 %34, 1
  %37 = extractvalue %dx.types.CBufRet.f32 %34, 2
  %38 = extractvalue %dx.types.CBufRet.f32 %34, 3
  %39 = fmul fast float %14, %20
  %40 = call float @dx.op.tertiary.f32(i32 46, float %15, float %25, float %39)  ; FMad(a,b,c)
  %41 = call float @dx.op.tertiary.f32(i32 46, float %16, float %30, float %40)  ; FMad(a,b,c)
  %42 = call float @dx.op.tertiary.f32(i32 46, float %17, float %35, float %41)  ; FMad(a,b,c)
  %43 = fmul fast float %14, %21
  %44 = call float @dx.op.tertiary.f32(i32 46, float %15, float %26, float %43)  ; FMad(a,b,c)
  %45 = call float @dx.op.tertiary.f32(i32 46, float %16, float %31, float %44)  ; FMad(a,b,c)
  %46 = call float @dx.op.tertiary.f32(i32 46, float %17, float %36, float %45)  ; FMad(a,b,c)
  %47 = fmul fast float %14, %22
  %48 = call float @dx.op.tertiary.f32(i32 46, float %15, float %27, float %47)  ; FMad(a,b,c)
  %49 = call float @dx.op.tertiary.f32(i32 46, float %16, float %32, float %48)  ; FMad(a,b,c)
  %50 = call float @dx.op.tertiary.f32(i32 46, float %17, float %37, float %49)  ; FMad(a,b,c)
  %51 = fmul fast float %14, %23
  %52 = call float @dx.op.tertiary.f32(i32 46, float %15, float %28, float %51)  ; FMad(a,b,c)
  %53 = call float @dx.op.tertiary.f32(i32 46, float %16, float %33, float %52)  ; FMad(a,b,c)
  %54 = call float @dx.op.tertiary.f32(i32 46, float %17, float %38, float %53)  ; FMad(a,b,c)
  %55 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0)
  %56 = call %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32 59, %dx.types.Handle %1, i32 4)  ; CBufferLoadLegacy(handle,regIndex)
  %57 = extractvalue %dx.types.CBufRet.f32 %56, 0
  %58 = extractvalue %dx.types.CBufRet.f32 %56, 1
  %59 = extractvalue %dx.types.CBufRet.f32 %56, 2
  %60 = extractvalue %dx.types.CBufRet.f32 %56, 3
  %61 = call %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32 59, %dx.types.Handle %1, i32 5)  ; CBufferLoadLegacy(handle,regIndex)
  %62 = extractvalue %dx.types.CBufRet.f32 %61, 0
  %63 = extractvalue %dx.types.CBufRet.f32 %61, 1
  %64 = extractvalue %dx.types.CBufRet.f32 %61, 2
  %65 = extractvalue %dx.types.CBufRet.f32 %61, 3
  %66 = call %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32 59, %dx.types.Handle %1, i32 6)  ; CBufferLoadLegacy(handle,regIndex)
  %67 = extractvalue %dx.types.CBufRet.f32 %66, 0
  %68 = extractvalue %dx.types.CBufRet.f32 %66, 1
  %69 = extractvalue %dx.types.CBufRet.f32 %66, 2
  %70 = extractvalue %dx.types.CBufRet.f32 %66, 3
  %71 = call %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32 59, %dx.types.Handle %1, i32 7)  ; CBufferLoadLegacy(handle,regIndex)
  %72 = extractvalue %dx.types.CBufRet.f32 %71, 0
  %73 = extractvalue %dx.types.CBufRet.f32 %71, 1
  %74 = extractvalue %dx.types.CBufRet.f32 %71, 2
  %75 = extractvalue %dx.types.CBufRet.f32 %71, 3
  %76 = fmul fast float %42, %57
  %77 = call float @dx.op.tertiary.f32(i32 46, float %46, float %62, float %76)  ; FMad(a,b,c)
  %78 = call float @dx.op.tertiary.f32(i32 46, float %50, float %67, float %77)  ; FMad(a,b,c)
  %79 = call float @dx.op.tertiary.f32(i32 46, float %54, float %72, float %78)  ; FMad(a,b,c)
  %80 = fmul fast float %42, %58
  %81 = call float @dx.op.tertiary.f32(i32 46, float %46, float %63, float %80)  ; FMad(a,b,c)
  %82 = call float @dx.op.tertiary.f32(i32 46, float %50, float %68, float %81)  ; FMad(a,b,c)
  %83 = call float @dx.op.tertiary.f32(i32 46, float %54, float %73, float %82)  ; FMad(a,b,c)
  %84 = fmul fast float %42, %59
  %85 = call float @dx.op.tertiary.f32(i32 46, float %46, float %64, float %84)  ; FMad(a,b,c)
  %86 = call float @dx.op.tertiary.f32(i32 46, float %50, float %69, float %85)  ; FMad(a,b,c)
  %87 = call float @dx.op.tertiary.f32(i32 46, float %54, float %74, float %86)  ; FMad(a,b,c)
  %88 = fmul fast float %42, %60
  %89 = call float @dx.op.tertiary.f32(i32 46, float %46, float %65, float %88)  ; FMad(a,b,c)
  %90 = call float @dx.op.tertiary.f32(i32 46, float %50, float %70, float %89)  ; FMad(a,b,c)
  %91 = call float @dx.op.tertiary.f32(i32 46, float %54, float %75, float %90)  ; FMad(a,b,c)
  %92 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0)
  %93 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0)
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 0, float %79)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 1, float %83)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 2, float %87)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 3, float %91)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  %94 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0)
  call void @dx.op.storeOutput.f32(i32 5, i32 1, i32 0, i8 0, float %7)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 1, i32 0, i8 1, float %8)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 1, i32 0, i8 2, float %9)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 1, i32 0, i8 3, float 1.000000e+00)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  %95 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0)
  %96 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0)
  %97 = call %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32 59, %dx.types.Handle %2, i32 0)  ; CBufferLoadLegacy(handle,regIndex)
  %98 = extractvalue %dx.types.CBufRet.f32 %97, 0
  %99 = extractvalue %dx.types.CBufRet.f32 %97, 1
  %100 = extractvalue %dx.types.CBufRet.f32 %97, 2
  %101 = extractvalue %dx.types.CBufRet.f32 %97, 3
  %102 = call %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32 59, %dx.types.Handle %2, i32 1)  ; CBufferLoadLegacy(handle,regIndex)
  %103 = extractvalue %dx.types.CBufRet.f32 %102, 0
  %104 = extractvalue %dx.types.CBufRet.f32 %102, 1
  %105 = extractvalue %dx.types.CBufRet.f32 %102, 2
  %106 = extractvalue %dx.types.CBufRet.f32 %102, 3
  %107 = call %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32 59, %dx.types.Handle %2, i32 2)  ; CBufferLoadLegacy(handle,regIndex)
  %108 = extractvalue %dx.types.CBufRet.f32 %107, 0
  %109 = extractvalue %dx.types.CBufRet.f32 %107, 1
  %110 = extractvalue %dx.types.CBufRet.f32 %107, 2
  %111 = extractvalue %dx.types.CBufRet.f32 %107, 3
  %112 = call %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32 59, %dx.types.Handle %2, i32 3)  ; CBufferLoadLegacy(handle,regIndex)
  %113 = extractvalue %dx.types.CBufRet.f32 %112, 0
  %114 = extractvalue %dx.types.CBufRet.f32 %112, 1
  %115 = extractvalue %dx.types.CBufRet.f32 %112, 2
  %116 = extractvalue %dx.types.CBufRet.f32 %112, 3
  %117 = fmul fast float %10, %98
  %118 = call float @dx.op.tertiary.f32(i32 46, float %11, float %103, float %117)  ; FMad(a,b,c)
  %119 = call float @dx.op.tertiary.f32(i32 46, float %12, float %108, float %118)  ; FMad(a,b,c)
  %120 = call float @dx.op.tertiary.f32(i32 46, float 1.000000e+00, float %113, float %119)  ; FMad(a,b,c)
  %121 = fmul fast float %10, %99
  %122 = call float @dx.op.tertiary.f32(i32 46, float %11, float %104, float %121)  ; FMad(a,b,c)
  %123 = call float @dx.op.tertiary.f32(i32 46, float %12, float %109, float %122)  ; FMad(a,b,c)
  %124 = call float @dx.op.tertiary.f32(i32 46, float 1.000000e+00, float %114, float %123)  ; FMad(a,b,c)
  %125 = fmul fast float %10, %100
  %126 = call float @dx.op.tertiary.f32(i32 46, float %11, float %105, float %125)  ; FMad(a,b,c)
  %127 = call float @dx.op.tertiary.f32(i32 46, float %12, float %110, float %126)  ; FMad(a,b,c)
  %128 = call float @dx.op.tertiary.f32(i32 46, float 1.000000e+00, float %115, float %127)  ; FMad(a,b,c)
  %129 = fmul fast float %10, %101
  %130 = call float @dx.op.tertiary.f32(i32 46, float %11, float %106, float %129)  ; FMad(a,b,c)
  %131 = call float @dx.op.tertiary.f32(i32 46, float %12, float %111, float %130)  ; FMad(a,b,c)
  %132 = call float @dx.op.tertiary.f32(i32 46, float 1.000000e+00, float %116, float %131)  ; FMad(a,b,c)
  %133 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0)
  %134 = call %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32 59, %dx.types.Handle %2, i32 4)  ; CBufferLoadLegacy(handle,regIndex)
  %135 = extractvalue %dx.types.CBufRet.f32 %134, 0
  %136 = extractvalue %dx.types.CBufRet.f32 %134, 1
  %137 = extractvalue %dx.types.CBufRet.f32 %134, 2
  %138 = extractvalue %dx.types.CBufRet.f32 %134, 3
  %139 = call %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32 59, %dx.types.Handle %2, i32 5)  ; CBufferLoadLegacy(handle,regIndex)
  %140 = extractvalue %dx.types.CBufRet.f32 %139, 0
  %141 = extractvalue %dx.types.CBufRet.f32 %139, 1
  %142 = extractvalue %dx.types.CBufRet.f32 %139, 2
  %143 = extractvalue %dx.types.CBufRet.f32 %139, 3
  %144 = call %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32 59, %dx.types.Handle %2, i32 6)  ; CBufferLoadLegacy(handle,regIndex)
  %145 = extractvalue %dx.types.CBufRet.f32 %144, 0
  %146 = extractvalue %dx.types.CBufRet.f32 %144, 1
  %147 = extractvalue %dx.types.CBufRet.f32 %144, 2
  %148 = extractvalue %dx.types.CBufRet.f32 %144, 3
  %149 = call %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32 59, %dx.types.Handle %2, i32 7)  ; CBufferLoadLegacy(handle,regIndex)
  %150 = extractvalue %dx.types.CBufRet.f32 %149, 0
  %151 = extractvalue %dx.types.CBufRet.f32 %149, 1
  %152 = extractvalue %dx.types.CBufRet.f32 %149, 2
  %153 = extractvalue %dx.types.CBufRet.f32 %149, 3
  %154 = fmul fast float %120, %135
  %155 = call float @dx.op.tertiary.f32(i32 46, float %124, float %140, float %154)  ; FMad(a,b,c)
  %156 = call float @dx.op.tertiary.f32(i32 46, float %128, float %145, float %155)  ; FMad(a,b,c)
  %157 = call float @dx.op.tertiary.f32(i32 46, float %132, float %150, float %156)  ; FMad(a,b,c)
  %158 = fmul fast float %120, %136
  %159 = call float @dx.op.tertiary.f32(i32 46, float %124, float %141, float %158)  ; FMad(a,b,c)
  %160 = call float @dx.op.tertiary.f32(i32 46, float %128, float %146, float %159)  ; FMad(a,b,c)
  %161 = call float @dx.op.tertiary.f32(i32 46, float %132, float %151, float %160)  ; FMad(a,b,c)
  %162 = fmul fast float %120, %137
  %163 = call float @dx.op.tertiary.f32(i32 46, float %124, float %142, float %162)  ; FMad(a,b,c)
  %164 = call float @dx.op.tertiary.f32(i32 46, float %128, float %147, float %163)  ; FMad(a,b,c)
  %165 = call float @dx.op.tertiary.f32(i32 46, float %132, float %152, float %164)  ; FMad(a,b,c)
  %166 = fmul fast float %120, %138
  %167 = call float @dx.op.tertiary.f32(i32 46, float %124, float %143, float %166)  ; FMad(a,b,c)
  %168 = call float @dx.op.tertiary.f32(i32 46, float %128, float %148, float %167)  ; FMad(a,b,c)
  %169 = call float @dx.op.tertiary.f32(i32 46, float %132, float %153, float %168)  ; FMad(a,b,c)
  %170 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0)
  %171 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0)
  call void @dx.op.storeOutput.f32(i32 5, i32 3, i32 0, i8 0, float %157)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 3, i32 0, i8 1, float %161)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 3, i32 0, i8 2, float %165)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 3, i32 0, i8 3, float %169)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  %172 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0)
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
!dx.viewIdState = !{!8}
!dx.entryPoints = !{!9}

!0 = !{!"clang version 3.7 (tags/RELEASE_370/final)"}
!1 = !{i32 1, i32 0}
!2 = !{i32 1, i32 5}
!3 = !{!"vs", i32 6, i32 0}
!4 = !{null, null, !5, null}
!5 = !{!6, !7}
!6 = !{i32 0, %hostlayout.Light_cb* undef, !"", i32 0, i32 0, i32 1, i32 132, null}
!7 = !{i32 1, %hostlayout.Camera_cb_v51* undef, !"", i32 0, i32 1, i32 1, i32 128, null}
!8 = !{[18 x i32] [i32 16, i32 16, i32 61455, i32 61455, i32 61455, i32 0, i32 16, i32 32, i32 64, i32 0, i32 15, i32 15, i32 15, i32 15, i32 0, i32 0, i32 0, i32 0]}
!9 = !{void ()* @main, !"main", !10, !4, !25}
!10 = !{!11, !20, null}
!11 = !{!12, !15, !16, !18}
!12 = !{i32 0, !"Position", i8 9, i8 0, !13, i8 0, i32 1, i8 3, i32 0, i8 0, !14}
!13 = !{i32 0}
!14 = !{i32 3, i32 7}
!15 = !{i32 1, !"Colour", i8 9, i8 0, !13, i8 0, i32 1, i8 3, i32 1, i8 0, !14}
!16 = !{i32 2, !"Instance_", i8 9, i8 0, !13, i8 0, i32 1, i8 4, i32 2, i8 0, !17}
!17 = !{i32 3, i32 15}
!18 = !{i32 3, !"Instance_", i8 9, i8 0, !19, i8 0, i32 1, i8 4, i32 3, i8 0, null}
!19 = !{i32 1}
!20 = !{!21, !22, !23, !24}
!21 = !{i32 0, !"SV_Position", i8 9, i8 3, !13, i8 4, i32 1, i8 4, i32 0, i8 0, !17}
!22 = !{i32 1, !"Colour", i8 9, i8 0, !13, i8 2, i32 1, i8 4, i32 1, i8 0, !17}
!23 = !{i32 2, !"Texcoord", i8 9, i8 0, !13, i8 2, i32 1, i8 4, i32 2, i8 0, null}
!24 = !{i32 3, !"Texcoord", i8 9, i8 0, !19, i8 2, i32 1, i8 4, i32 3, i8 0, !17}
!25 = !{i32 0, i64 1}
