; ModuleID = 'app3.c'
source_filename = "app3.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

; Function Attrs: noreturn nounwind sspstrong uwtable
define dso_local void @app() local_unnamed_addr #0 {
  %1 = alloca [2 x [85 x [170 x i32]]], align 16
  %2 = alloca [4 x i32], align 16
  %3 = alloca [4 x i32], align 16
  %4 = alloca [4 x i32], align 16
  %5 = alloca [4 x i32], align 16
  %6 = alloca [4 x i32], align 16
  %7 = alloca [4 x i32], align 16
  call void @llvm.lifetime.start.p0(i64 115600, ptr nonnull %1) #5
  call void @llvm.memset.p0.i64(ptr noundef nonnull align 16 dereferenceable(57800) %1, i8 0, i64 57800, i1 false), !tbaa !5
  call void @llvm.lifetime.start.p0(i64 16, ptr nonnull %2) #5
  call void @llvm.lifetime.start.p0(i64 16, ptr nonnull %3) #5
  call void @llvm.lifetime.start.p0(i64 16, ptr nonnull %4) #5
  call void @llvm.lifetime.start.p0(i64 16, ptr nonnull %5) #5
  call void @llvm.lifetime.start.p0(i64 16, ptr nonnull %6) #5
  call void @llvm.lifetime.start.p0(i64 16, ptr nonnull %7) #5
  br label %8

8:                                                ; preds = %0, %8
  %9 = phi i64 [ 0, %0 ], [ %41, %8 ]
  %10 = tail call i32 (...) @simRand() #5
  %11 = srem i32 %10, 9
  %12 = add nsw i32 %11, 4
  %13 = getelementptr inbounds nuw [4 x i32], ptr %6, i64 0, i64 %9
  store i32 %12, ptr %13, align 4, !tbaa !5
  %14 = tail call i32 (...) @simRand() #5
  %15 = and i32 %14, 63
  %16 = add nuw nsw i32 %15, 176
  %17 = getelementptr inbounds nuw [4 x i32], ptr %7, i64 0, i64 %9
  store i32 %16, ptr %17, align 4, !tbaa !5
  %18 = add nsw i32 %11, 5
  %19 = tail call i32 (...) @simRand() #5
  %20 = add nsw i32 %11, %18
  %21 = sub nsw i32 165, %20
  %22 = srem i32 %19, %21
  %23 = add nsw i32 %22, %18
  %24 = getelementptr inbounds nuw [4 x i32], ptr %2, i64 0, i64 %9
  store i32 %23, ptr %24, align 4, !tbaa !5
  %25 = tail call i32 (...) @simRand() #5
  %26 = add nsw i32 %11, %18
  %27 = sub nsw i32 80, %26
  %28 = srem i32 %25, %27
  %29 = add nsw i32 %28, %18
  %30 = getelementptr inbounds nuw [4 x i32], ptr %3, i64 0, i64 %9
  store i32 %29, ptr %30, align 4, !tbaa !5
  %31 = tail call i32 (...) @simRand() #5
  %32 = and i32 %31, 1
  %33 = icmp eq i32 %32, 0
  %34 = select i1 %33, i32 -1, i32 1
  %35 = getelementptr inbounds nuw [4 x i32], ptr %4, i64 0, i64 %9
  store i32 %34, ptr %35, align 4, !tbaa !5
  %36 = tail call i32 (...) @simRand() #5
  %37 = and i32 %36, 1
  %38 = icmp eq i32 %37, 0
  %39 = select i1 %38, i32 -1, i32 1
  %40 = getelementptr inbounds nuw [4 x i32], ptr %5, i64 0, i64 %9
  store i32 %39, ptr %40, align 4, !tbaa !5
  %41 = add nuw nsw i64 %9, 1
  %42 = icmp eq i64 %41, 4
  br i1 %42, label %43, label %8, !llvm.loop !9

43:                                               ; preds = %8, %191
  %44 = phi i32 [ %83, %191 ], [ 1, %8 ]
  %45 = phi i32 [ %84, %191 ], [ 0, %8 ]
  br label %46

46:                                               ; preds = %43, %76
  %47 = phi i64 [ 0, %43 ], [ %77, %76 ]
  %48 = getelementptr inbounds nuw [4 x i32], ptr %4, i64 0, i64 %47
  %49 = load i32, ptr %48, align 4, !tbaa !5
  %50 = getelementptr inbounds nuw [4 x i32], ptr %2, i64 0, i64 %47
  %51 = load i32, ptr %50, align 4, !tbaa !5
  %52 = add nsw i32 %51, %49
  store i32 %52, ptr %50, align 4, !tbaa !5
  %53 = getelementptr inbounds nuw [4 x i32], ptr %5, i64 0, i64 %47
  %54 = load i32, ptr %53, align 4, !tbaa !5
  %55 = getelementptr inbounds nuw [4 x i32], ptr %3, i64 0, i64 %47
  %56 = load i32, ptr %55, align 4, !tbaa !5
  %57 = add nsw i32 %56, %54
  store i32 %57, ptr %55, align 4, !tbaa !5
  %58 = getelementptr inbounds nuw [4 x i32], ptr %6, i64 0, i64 %47
  %59 = load i32, ptr %58, align 4, !tbaa !5
  %60 = add nsw i32 %59, 1
  %61 = sub nsw i32 83, %59
  %62 = icmp sgt i32 %52, %60
  br i1 %62, label %63, label %66

63:                                               ; preds = %46
  %64 = sub nsw i32 168, %59
  %65 = icmp slt i32 %52, %64
  br i1 %65, label %69, label %66

66:                                               ; preds = %63, %46
  %67 = phi i32 [ %60, %46 ], [ %64, %63 ]
  store i32 %67, ptr %50, align 4, !tbaa !5
  %68 = sub nsw i32 0, %49
  store i32 %68, ptr %48, align 4, !tbaa !5
  br label %69

69:                                               ; preds = %66, %63
  %70 = icmp sgt i32 %57, %60
  br i1 %70, label %71, label %73

71:                                               ; preds = %69
  %72 = icmp slt i32 %57, %61
  br i1 %72, label %76, label %73

73:                                               ; preds = %71, %69
  %74 = phi i32 [ %60, %69 ], [ %61, %71 ]
  store i32 %74, ptr %55, align 4, !tbaa !5
  %75 = sub nsw i32 0, %54
  store i32 %75, ptr %53, align 4, !tbaa !5
  br label %76

76:                                               ; preds = %73, %71
  %77 = add nuw nsw i64 %47, 1
  %78 = icmp eq i64 %77, 4
  br i1 %78, label %82, label %46, !llvm.loop !12

79:                                               ; preds = %178
  %80 = zext nneg i32 %84 to i64
  %81 = getelementptr inbounds nuw [2 x [85 x [170 x i32]]], ptr %1, i64 0, i64 %80
  br label %187

82:                                               ; preds = %76, %178
  %83 = phi i32 [ %84, %178 ], [ %45, %76 ]
  %84 = phi i32 [ %83, %178 ], [ %44, %76 ]
  %85 = phi i32 [ %179, %178 ], [ 0, %76 ]
  %86 = zext nneg i32 %83 to i64
  %87 = getelementptr inbounds nuw [2 x [85 x [170 x i32]]], ptr %1, i64 0, i64 %86
  %88 = zext nneg i32 %84 to i64
  br label %89

89:                                               ; preds = %82, %114
  %90 = phi i64 [ 0, %82 ], [ %115, %114 ]
  %91 = getelementptr inbounds nuw [4 x i32], ptr %3, i64 0, i64 %90
  %92 = load i32, ptr %91, align 4, !tbaa !5
  %93 = getelementptr inbounds nuw [4 x i32], ptr %6, i64 0, i64 %90
  %94 = load i32, ptr %93, align 4, !tbaa !5
  %95 = mul nsw i32 %94, %94
  %96 = sub nsw i32 %92, %94
  %97 = tail call i32 @llvm.smax.i32(i32 %96, i32 1)
  %98 = add nsw i32 %94, %92
  %99 = tail call i32 @llvm.smin.i32(i32 %98, i32 83)
  %100 = icmp sgt i32 %97, %99
  br i1 %100, label %114, label %101

101:                                              ; preds = %89
  %102 = getelementptr inbounds nuw [4 x i32], ptr %2, i64 0, i64 %90
  %103 = load i32, ptr %102, align 4, !tbaa !5
  %104 = add nsw i32 %94, %103
  %105 = tail call i32 @llvm.smin.i32(i32 %104, i32 168)
  %106 = sub i32 %103, %94
  %107 = tail call i32 @llvm.smax.i32(i32 %106, i32 1)
  %108 = icmp sgt i32 %107, %105
  %109 = getelementptr inbounds nuw [4 x i32], ptr %7, i64 0, i64 %90
  %110 = zext nneg i32 %107 to i64
  %111 = sext i32 %105 to i64
  %112 = zext nneg i32 %97 to i64
  %113 = sext i32 %99 to i64
  br label %119

114:                                              ; preds = %124, %89
  %115 = add nuw nsw i64 %90, 1
  %116 = icmp eq i64 %115, 4
  br i1 %116, label %117, label %89, !llvm.loop !13

117:                                              ; preds = %114
  %118 = getelementptr inbounds nuw [2 x [85 x [170 x i32]]], ptr %1, i64 0, i64 %88
  br label %144

119:                                              ; preds = %101, %124
  %120 = phi i64 [ %112, %101 ], [ %125, %124 ]
  %121 = trunc i64 %120 to i32
  %122 = sub i32 %121, %92
  %123 = mul nsw i32 %122, %122
  br i1 %108, label %124, label %127

124:                                              ; preds = %139, %119
  %125 = add nuw nsw i64 %120, 1
  %126 = icmp slt i64 %120, %113
  br i1 %126, label %119, label %114, !llvm.loop !14

127:                                              ; preds = %119, %139
  %128 = phi i64 [ %140, %139 ], [ %110, %119 ]
  %129 = trunc i64 %128 to i32
  %130 = sub i32 %129, %103
  %131 = mul nsw i32 %130, %130
  %132 = add nuw nsw i32 %131, %123
  %133 = icmp samesign ugt i32 %132, %95
  br i1 %133, label %139, label %134

134:                                              ; preds = %127
  %135 = load i32, ptr %109, align 4, !tbaa !5
  %136 = getelementptr inbounds nuw [170 x i32], ptr %87, i64 %120, i64 %128
  %137 = load i32, ptr %136, align 4, !tbaa !5
  %138 = tail call i32 @llvm.smax.i32(i32 %137, i32 %135)
  store i32 %138, ptr %136, align 4
  br label %139

139:                                              ; preds = %134, %127
  %140 = add nuw nsw i64 %128, 1
  %141 = icmp slt i64 %128, %111
  br i1 %141, label %127, label %124, !llvm.loop !15

142:                                              ; preds = %149
  %143 = getelementptr inbounds nuw i8, ptr %118, i64 57120
  call void @llvm.memset.p0.i64(ptr noundef nonnull align 8 dereferenceable(680) %118, i8 0, i64 680, i1 false), !tbaa !5
  call void @llvm.memset.p0.i64(ptr noundef nonnull align 8 dereferenceable(680) %143, i8 0, i64 680, i1 false), !tbaa !5
  br label %181

144:                                              ; preds = %117, %149
  %145 = phi i64 [ 1, %117 ], [ %150, %149 ]
  %146 = getelementptr [170 x i32], ptr %87, i64 %145
  %147 = getelementptr i8, ptr %146, i64 -680
  %148 = getelementptr inbounds nuw i8, ptr %146, i64 680
  br label %152

149:                                              ; preds = %152
  %150 = add nuw nsw i64 %145, 1
  %151 = icmp eq i64 %150, 84
  br i1 %151, label %142, label %144, !llvm.loop !16

152:                                              ; preds = %144, %152
  %153 = phi i64 [ 1, %144 ], [ %159, %152 ]
  %154 = getelementptr inbounds nuw [170 x i32], ptr %87, i64 %145, i64 %153
  %155 = load i32, ptr %154, align 4, !tbaa !5
  %156 = add nsw i64 %153, -1
  %157 = getelementptr inbounds [170 x i32], ptr %87, i64 %145, i64 %156
  %158 = load i32, ptr %157, align 4, !tbaa !5
  %159 = add nuw nsw i64 %153, 1
  %160 = getelementptr inbounds nuw [170 x i32], ptr %87, i64 %145, i64 %159
  %161 = load i32, ptr %160, align 4, !tbaa !5
  %162 = getelementptr inbounds nuw [170 x i32], ptr %147, i64 0, i64 %153
  %163 = load i32, ptr %162, align 4, !tbaa !5
  %164 = getelementptr inbounds nuw [170 x i32], ptr %148, i64 0, i64 %153
  %165 = load i32, ptr %164, align 4, !tbaa !5
  %166 = shl i32 %155, 2
  %167 = sub i32 %158, %166
  %168 = add i32 %167, %161
  %169 = add i32 %168, %163
  %170 = add i32 %169, %165
  %171 = ashr i32 %170, 2
  %172 = add nsw i32 %171, %155
  %173 = tail call i32 @llvm.smax.i32(i32 %172, i32 1)
  %174 = tail call i32 @llvm.smin.i32(i32 %173, i32 256)
  %175 = add nsw i32 %174, -1
  %176 = getelementptr inbounds nuw [170 x i32], ptr %118, i64 %145, i64 %153
  store i32 %175, ptr %176, align 4, !tbaa !5
  %177 = icmp eq i64 %159, 169
  br i1 %177, label %149, label %152, !llvm.loop !17

178:                                              ; preds = %181
  %179 = add nuw nsw i32 %85, 1
  %180 = icmp eq i32 %179, 4
  br i1 %180, label %79, label %82, !llvm.loop !18

181:                                              ; preds = %142, %181
  %182 = phi i64 [ 0, %142 ], [ %185, %181 ]
  %183 = getelementptr inbounds nuw [170 x i32], ptr %118, i64 %182
  store i32 0, ptr %183, align 8, !tbaa !5
  %184 = getelementptr inbounds nuw i8, ptr %183, i64 676
  store i32 0, ptr %184, align 4, !tbaa !5
  %185 = add nuw nsw i64 %182, 1
  %186 = icmp eq i64 %185, 85
  br i1 %186, label %178, label %181, !llvm.loop !19

187:                                              ; preds = %79, %192
  %188 = phi i64 [ 0, %79 ], [ %193, %192 ]
  %189 = trunc i64 %188 to i32
  %190 = mul i32 %189, 3
  br label %195

191:                                              ; preds = %192
  tail call void (...) @simFlush() #5
  br label %43, !llvm.loop !20

192:                                              ; preds = %220
  %193 = add nuw nsw i64 %188, 1
  %194 = icmp eq i64 %193, 85
  br i1 %194, label %191, label %187, !llvm.loop !21

195:                                              ; preds = %187, %220
  %196 = phi i64 [ 0, %187 ], [ %221, %220 ]
  %197 = getelementptr inbounds nuw [170 x i32], ptr %81, i64 %188, i64 %196
  %198 = load i32, ptr %197, align 4, !tbaa !5
  %199 = tail call i32 @llvm.smax.i32(i32 %198, i32 0)
  %200 = tail call i32 @llvm.umin.i32(i32 %199, i32 255)
  %201 = shl nuw nsw i32 %200, 16
  %202 = shl nuw nsw i32 %200, 7
  %203 = and i32 %202, 32512
  %204 = or disjoint i32 %203, %201
  %205 = or disjoint i32 %204, %200
  %206 = xor i32 %205, -16776961
  %207 = trunc i64 %196 to i32
  %208 = mul i32 %207, 3
  br label %209

209:                                              ; preds = %195, %217
  %210 = phi i32 [ 0, %195 ], [ %218, %217 ]
  %211 = add nuw nsw i32 %210, %190
  br label %212

212:                                              ; preds = %209, %212
  %213 = phi i32 [ 0, %209 ], [ %215, %212 ]
  %214 = add nuw nsw i32 %213, %208
  tail call void @simPutPixel(i32 noundef %214, i32 noundef %211, i32 noundef %206) #5
  %215 = add nuw nsw i32 %213, 1
  %216 = icmp eq i32 %215, 3
  br i1 %216, label %217, label %212, !llvm.loop !22

217:                                              ; preds = %212
  %218 = add nuw nsw i32 %210, 1
  %219 = icmp eq i32 %218, 3
  br i1 %219, label %220, label %209, !llvm.loop !23

220:                                              ; preds = %217
  %221 = add nuw nsw i64 %196, 1
  %222 = icmp eq i64 %221, 170
  br i1 %222, label %192, label %195, !llvm.loop !24
}

; Function Attrs: mustprogress nocallback nofree nosync nounwind willreturn memory(argmem: readwrite)
declare void @llvm.lifetime.start.p0(i64 immarg, ptr nocapture) #1

declare i32 @simRand(...) local_unnamed_addr #2

declare void @simPutPixel(i32 noundef, i32 noundef, i32 noundef) local_unnamed_addr #2

declare void @simFlush(...) local_unnamed_addr #2

; Function Attrs: nocallback nofree nosync nounwind speculatable willreturn memory(none)
declare i32 @llvm.smax.i32(i32, i32) #3

; Function Attrs: nocallback nofree nosync nounwind speculatable willreturn memory(none)
declare i32 @llvm.umin.i32(i32, i32) #3

; Function Attrs: nocallback nofree nosync nounwind speculatable willreturn memory(none)
declare i32 @llvm.smin.i32(i32, i32) #3

; Function Attrs: nocallback nofree nounwind willreturn memory(argmem: write)
declare void @llvm.memset.p0.i64(ptr nocapture writeonly, i8, i64, i1 immarg) #4

attributes #0 = { noreturn nounwind sspstrong uwtable "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #1 = { mustprogress nocallback nofree nosync nounwind willreturn memory(argmem: readwrite) }
attributes #2 = { "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #3 = { nocallback nofree nosync nounwind speculatable willreturn memory(none) }
attributes #4 = { nocallback nofree nounwind willreturn memory(argmem: write) }
attributes #5 = { nounwind }

!llvm.module.flags = !{!0, !1, !2, !3}
!llvm.ident = !{!4}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{i32 8, !"PIC Level", i32 2}
!2 = !{i32 7, !"PIE Level", i32 2}
!3 = !{i32 7, !"uwtable", i32 2}
!4 = !{!"clang version 20.1.8"}
!5 = !{!6, !6, i64 0}
!6 = !{!"int", !7, i64 0}
!7 = !{!"omnipotent char", !8, i64 0}
!8 = !{!"Simple C/C++ TBAA"}
!9 = distinct !{!9, !10, !11}
!10 = !{!"llvm.loop.mustprogress"}
!11 = !{!"llvm.loop.unroll.disable"}
!12 = distinct !{!12, !10, !11}
!13 = distinct !{!13, !10, !11}
!14 = distinct !{!14, !10, !11}
!15 = distinct !{!15, !10, !11}
!16 = distinct !{!16, !10, !11}
!17 = distinct !{!17, !10, !11}
!18 = distinct !{!18, !10, !11}
!19 = distinct !{!19, !10, !11}
!20 = distinct !{!20, !11}
!21 = distinct !{!21, !10, !11}
!22 = distinct !{!22, !10, !11}
!23 = distinct !{!23, !10, !11}
!24 = distinct !{!24, !10, !11}
