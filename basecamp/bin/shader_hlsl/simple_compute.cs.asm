;
; Note: shader requires additional functionality:
;       Typed UAV Load Additional Formats
;
;
; Input signature:
;
; Name                 Index   Mask Register SysValue  Format   Used
; -------------------- ----- ------ -------- -------- ------- ------
; no parameters
;
; Output signature:
;
; Name                 Index   Mask Register SysValue  Format   Used
; -------------------- ----- ------ -------- -------- ------- ------
; no parameters
; shader hash: d07fa1c5cff964ede6b972792b923910
;
; Pipeline Runtime Information: 
;
;
;
; Buffer Definitions:
;
;
; Resource Bindings:
;
; Name                                 Type  Format         Dim      ID      HLSL Bind  Count
; ------------------------------ ---------- ------- ----------- ------- -------------- ------
; Texture_srv                       texture     f32          2d      T0             t0     1
; Texture_uav                           UAV     f32          2d      U0             u0     1
;
target datalayout = "e-m:e-p:32:32-i1:32-i8:32-i16:32-i32:32-i64:64-f16:32-f32:32-f64:64-n8:16:32:64"
target triple = "dxil-ms-dx"

%dx.types.Handle = type { i8* }
%dx.types.Dimensions = type { i32, i32, i32, i32 }
%dx.types.ResRet.f32 = type { float, float, float, float, i32 }
%"class.Texture2D<vector<float, 4> >" = type { <4 x float>, %"class.Texture2D<vector<float, 4> >::mips_type" }
%"class.Texture2D<vector<float, 4> >::mips_type" = type { i32 }
%"class.RWTexture2D<vector<float, 4> >" = type { <4 x float> }

@dx.nothing.a = internal constant [1 x i32] zeroinitializer

define void @main() {
  %1 = call %dx.types.Handle @dx.op.createHandle(i32 57, i8 1, i32 0, i32 0, i1 false)  ; CreateHandle(resourceClass,rangeId,index,nonUniformIndex)
  %2 = call %dx.types.Handle @dx.op.createHandle(i32 57, i8 0, i32 0, i32 0, i1 false)  ; CreateHandle(resourceClass,rangeId,index,nonUniformIndex)
  %3 = call i32 @dx.op.threadId.i32(i32 93, i32 0)  ; ThreadId(component)
  %4 = call i32 @dx.op.threadId.i32(i32 93, i32 1)  ; ThreadId(component)
  %5 = alloca i32, align 4
  %6 = alloca i32, align 4
  %7 = call %dx.types.Dimensions @dx.op.getDimensions(i32 72, %dx.types.Handle %1, i32 0)  ; GetDimensions(handle,mipLevel)
  %8 = extractvalue %dx.types.Dimensions %7, 0
  store i32 %8, i32* %5
  %9 = extractvalue %dx.types.Dimensions %7, 1
  store i32 %9, i32* %6
  %10 = load i32, i32* %5, align 4
  %11 = icmp uge i32 %3, %10
  %12 = icmp ne i1 %11, false
  %13 = load i32, i32* %6, align 4
  %14 = icmp uge i32 %4, %13
  %15 = icmp ne i1 %14, false
  %16 = or i1 %12, %15
  %17 = icmp ne i1 %16, false
  br i1 %17, label %18, label %19

; <label>:18                                      ; preds = %0
  br label %55

; <label>:19                                      ; preds = %0
  %20 = uitofp i32 %3 to float
  %21 = load i32, i32* %5, align 4
  %22 = uitofp i32 %21 to float
  %23 = fmul fast float %22, 0x3FE051EB80000000
  %24 = fcmp fast ogt float %20, %23
  %25 = icmp ne i1 %24, false
  %26 = icmp ne i1 %25, false
  br i1 %26, label %27, label %34

; <label>:27                                      ; preds = %19
  %28 = call %dx.types.ResRet.f32 @dx.op.textureLoad.f32(i32 66, %dx.types.Handle %2, i32 0, i32 %3, i32 %4, i32 undef, i32 undef, i32 undef, i32 undef)  ; TextureLoad(srv,mipLevelOrSampleCount,coord0,coord1,coord2,offset0,offset1,offset2)
  %29 = extractvalue %dx.types.ResRet.f32 %28, 0
  %30 = extractvalue %dx.types.ResRet.f32 %28, 1
  %31 = extractvalue %dx.types.ResRet.f32 %28, 2
  %32 = extractvalue %dx.types.ResRet.f32 %28, 3
  %33 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0)
  call void @dx.op.textureStore.f32(i32 67, %dx.types.Handle %1, i32 %3, i32 %4, i32 undef, float %29, float %30, float %31, float %32, i8 15)  ; TextureStore(srv,coord0,coord1,coord2,value0,value1,value2,value3,mask)
  br label %55

; <label>:34                                      ; preds = %19
  %35 = uitofp i32 %3 to float
  %36 = load i32, i32* %5, align 4
  %37 = uitofp i32 %36 to float
  %38 = fmul fast float %37, 0x3FDF5C2900000000
  %39 = fcmp fast ogt float %35, %38
  %40 = icmp ne i1 %39, false
  %41 = icmp ne i1 %40, false
  br i1 %41, label %42, label %54

; <label>:42                                      ; preds = %34
  %43 = call %dx.types.ResRet.f32 @dx.op.textureLoad.f32(i32 66, %dx.types.Handle %1, i32 undef, i32 %3, i32 %4, i32 undef, i32 undef, i32 undef, i32 undef)  ; TextureLoad(srv,mipLevelOrSampleCount,coord0,coord1,coord2,offset0,offset1,offset2)
  %44 = extractvalue %dx.types.ResRet.f32 %43, 0
  %45 = extractvalue %dx.types.ResRet.f32 %43, 1
  %46 = extractvalue %dx.types.ResRet.f32 %43, 2
  %47 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0)
  %48 = fmul fast float %44, 0x3FD3333340000000
  %49 = fmul fast float %45, 0x3FE2E147A0000000
  %50 = fadd fast float %48, %49
  %51 = fmul fast float %46, 0x3FBC28F5C0000000
  %52 = fadd fast float %50, %51
  %53 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0)
  call void @dx.op.textureStore.f32(i32 67, %dx.types.Handle %1, i32 %3, i32 %4, i32 undef, float %52, float %52, float %52, float 1.000000e+00, i8 15)  ; TextureStore(srv,coord0,coord1,coord2,value0,value1,value2,value3,mask)
  br label %55

; <label>:54                                      ; preds = %34
  br label %55

; <label>:55                                      ; preds = %54, %42, %27, %18
  %56 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0)
  ret void
}

; Function Attrs: nounwind readnone
declare i32 @dx.op.threadId.i32(i32, i32) #0

; Function Attrs: nounwind readonly
declare %dx.types.Dimensions @dx.op.getDimensions(i32, %dx.types.Handle, i32) #1

; Function Attrs: nounwind readonly
declare %dx.types.ResRet.f32 @dx.op.textureLoad.f32(i32, %dx.types.Handle, i32, i32, i32, i32, i32, i32, i32) #1

; Function Attrs: nounwind
declare void @dx.op.textureStore.f32(i32, %dx.types.Handle, i32, i32, i32, float, float, float, float, i8) #2

; Function Attrs: nounwind readonly
declare %dx.types.Handle @dx.op.createHandle(i32, i8, i32, i32, i1) #1

attributes #0 = { nounwind readnone }
attributes #1 = { nounwind readonly }
attributes #2 = { nounwind }

!llvm.ident = !{!0}
!dx.version = !{!1}
!dx.valver = !{!2}
!dx.shaderModel = !{!3}
!dx.resources = !{!4}
!dx.entryPoints = !{!10}

!0 = !{!"clang version 3.7 (tags/RELEASE_370/final)"}
!1 = !{i32 1, i32 0}
!2 = !{i32 1, i32 5}
!3 = !{!"cs", i32 6, i32 0}
!4 = !{!5, !8, null, null}
!5 = !{!6}
!6 = !{i32 0, %"class.Texture2D<vector<float, 4> >"* undef, !"", i32 0, i32 0, i32 1, i32 2, i32 0, !7}
!7 = !{i32 0, i32 9}
!8 = !{!9}
!9 = !{i32 0, %"class.RWTexture2D<vector<float, 4> >"* undef, !"", i32 0, i32 0, i32 1, i32 2, i1 false, i1 false, i1 false, !7}
!10 = !{void ()* @main, !"main", null, !4, !11}
!11 = !{i32 0, i64 8193, i32 4, !12}
!12 = !{i32 32, i32 32, i32 1}
