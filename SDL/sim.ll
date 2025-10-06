; ModuleID = 'sim.c'
source_filename = "sim.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

%union.SDL_Event = type { %struct.SDL_SensorEvent, [8 x i8] }
%struct.SDL_SensorEvent = type { i32, i32, i32, [6 x float], i64 }

@Window = internal global ptr null, align 8
@Renderer = internal global ptr null, align 8
@.str.1 = private unnamed_addr constant [60 x i8] c"SDL_TRUE != SDL_HasEvent(SDL_QUIT) && \22User-requested quit\22\00", align 1
@.str.2 = private unnamed_addr constant [6 x i8] c"sim.c\00", align 1
@__PRETTY_FUNCTION__.simFlush = private unnamed_addr constant [16 x i8] c"void simFlush()\00", align 1
@Ticks = internal unnamed_addr global i32 0, align 4
@.str.4 = private unnamed_addr constant [43 x i8] c"0 <= x && x < SIM_X_SIZE && \22Out of range\22\00", align 1
@__PRETTY_FUNCTION__.simPutPixel = private unnamed_addr constant [32 x i8] c"void simPutPixel(int, int, int)\00", align 1
@.str.5 = private unnamed_addr constant [43 x i8] c"0 <= y && y < SIM_Y_SIZE && \22Out of range\22\00", align 1

; Function Attrs: nounwind sspstrong uwtable
define dso_local void @simInit() local_unnamed_addr #0 {
  %1 = tail call i32 @SDL_Init(i32 noundef 32) #5
  %2 = tail call i32 @SDL_CreateWindowAndRenderer(i32 noundef 512, i32 noundef 256, i32 noundef 0, ptr noundef nonnull @Window, ptr noundef nonnull @Renderer) #5
  %3 = load ptr, ptr @Renderer, align 8, !tbaa !5
  %4 = tail call i32 @SDL_SetRenderDrawColor(ptr noundef %3, i8 noundef zeroext 0, i8 noundef zeroext 0, i8 noundef zeroext 0, i8 noundef zeroext 0) #5
  %5 = load ptr, ptr @Renderer, align 8, !tbaa !5
  %6 = tail call i32 @SDL_RenderClear(ptr noundef %5) #5
  %7 = tail call i64 @time(ptr noundef null) #5
  %8 = trunc i64 %7 to i32
  tail call void @srand(i32 noundef %8) #5
  %9 = load ptr, ptr @Renderer, align 8, !tbaa !5
  %10 = tail call i32 @SDL_SetRenderDrawColor(ptr noundef %9, i8 noundef zeroext 0, i8 noundef zeroext 0, i8 noundef zeroext 0, i8 noundef zeroext 0) #5
  %11 = load ptr, ptr @Renderer, align 8, !tbaa !5
  %12 = tail call i32 @SDL_RenderDrawPoint(ptr noundef %11, i32 noundef 0, i32 noundef 0) #5
  %13 = tail call i32 @SDL_GetTicks() #5
  store i32 %13, ptr @Ticks, align 4, !tbaa !10
  tail call void @simFlush()
  ret void
}

declare i32 @SDL_Init(i32 noundef) local_unnamed_addr #1

declare i32 @SDL_CreateWindowAndRenderer(i32 noundef, i32 noundef, i32 noundef, ptr noundef, ptr noundef) local_unnamed_addr #1

declare i32 @SDL_SetRenderDrawColor(ptr noundef, i8 noundef zeroext, i8 noundef zeroext, i8 noundef zeroext, i8 noundef zeroext) local_unnamed_addr #1

declare i32 @SDL_RenderClear(ptr noundef) local_unnamed_addr #1

; Function Attrs: nounwind
declare void @srand(i32 noundef) local_unnamed_addr #2

; Function Attrs: nounwind
declare i64 @time(ptr noundef) local_unnamed_addr #2

; Function Attrs: nounwind sspstrong uwtable
define dso_local void @simPutPixel(i32 noundef %0, i32 noundef %1, i32 noundef %2) local_unnamed_addr #0 {
  %4 = icmp ult i32 %0, 512
  br i1 %4, label %6, label %5

5:                                                ; preds = %3
  tail call void @__assert_fail(ptr noundef nonnull @.str.4, ptr noundef nonnull @.str.2, i32 noundef 51, ptr noundef nonnull @__PRETTY_FUNCTION__.simPutPixel) #6
  unreachable

6:                                                ; preds = %3
  %7 = icmp ult i32 %1, 256
  br i1 %7, label %9, label %8

8:                                                ; preds = %6
  tail call void @__assert_fail(ptr noundef nonnull @.str.5, ptr noundef nonnull @.str.2, i32 noundef 52, ptr noundef nonnull @__PRETTY_FUNCTION__.simPutPixel) #6
  unreachable

9:                                                ; preds = %6
  %10 = lshr i32 %2, 24
  %11 = trunc nuw i32 %10 to i8
  %12 = lshr i32 %2, 16
  %13 = trunc i32 %12 to i8
  %14 = lshr i32 %2, 8
  %15 = trunc i32 %14 to i8
  %16 = trunc i32 %2 to i8
  %17 = load ptr, ptr @Renderer, align 8, !tbaa !5
  %18 = tail call i32 @SDL_SetRenderDrawColor(ptr noundef %17, i8 noundef zeroext %13, i8 noundef zeroext %15, i8 noundef zeroext %16, i8 noundef zeroext %11) #5
  %19 = load ptr, ptr @Renderer, align 8, !tbaa !5
  %20 = tail call i32 @SDL_RenderDrawPoint(ptr noundef %19, i32 noundef %0, i32 noundef %1) #5
  %21 = tail call i32 @SDL_GetTicks() #5
  store i32 %21, ptr @Ticks, align 4, !tbaa !10
  ret void
}

; Function Attrs: nounwind sspstrong uwtable
define dso_local void @simExit() local_unnamed_addr #0 {
  %1 = alloca %union.SDL_Event, align 8
  call void @llvm.lifetime.start.p0(i64 56, ptr nonnull %1) #5
  br label %2

2:                                                ; preds = %2, %0
  %3 = call i32 @SDL_PollEvent(ptr noundef nonnull %1) #5
  %4 = icmp ne i32 %3, 0
  %5 = load i32, ptr %1, align 8
  %6 = icmp eq i32 %5, 256
  %7 = select i1 %4, i1 %6, i1 false
  br i1 %7, label %8, label %2, !llvm.loop !12

8:                                                ; preds = %2
  %9 = load ptr, ptr @Renderer, align 8, !tbaa !5
  call void @SDL_DestroyRenderer(ptr noundef %9) #5
  %10 = load ptr, ptr @Window, align 8, !tbaa !14
  call void @SDL_DestroyWindow(ptr noundef %10) #5
  call void @SDL_Quit() #5
  call void @llvm.lifetime.end.p0(i64 56, ptr nonnull %1) #5
  ret void
}

; Function Attrs: mustprogress nocallback nofree nosync nounwind willreturn memory(argmem: readwrite)
declare void @llvm.lifetime.start.p0(i64 immarg, ptr nocapture) #3

declare i32 @SDL_PollEvent(ptr noundef) local_unnamed_addr #1

declare void @SDL_DestroyRenderer(ptr noundef) local_unnamed_addr #1

declare void @SDL_DestroyWindow(ptr noundef) local_unnamed_addr #1

declare void @SDL_Quit() local_unnamed_addr #1

; Function Attrs: mustprogress nocallback nofree nosync nounwind willreturn memory(argmem: readwrite)
declare void @llvm.lifetime.end.p0(i64 immarg, ptr nocapture) #3

; Function Attrs: nounwind sspstrong uwtable
define dso_local void @simFlush() local_unnamed_addr #0 {
  tail call void @SDL_PumpEvents() #5
  %1 = tail call i32 @SDL_HasEvent(i32 noundef 256) #5
  %2 = icmp eq i32 %1, 1
  br i1 %2, label %3, label %4

3:                                                ; preds = %0
  tail call void @__assert_fail(ptr noundef nonnull @.str.1, ptr noundef nonnull @.str.2, i32 noundef 40, ptr noundef nonnull @__PRETTY_FUNCTION__.simFlush) #6
  unreachable

4:                                                ; preds = %0
  %5 = tail call i32 @SDL_GetTicks() #5
  %6 = load i32, ptr @Ticks, align 4, !tbaa !10
  %7 = sub i32 %5, %6
  %8 = icmp ult i32 %7, 50
  br i1 %8, label %9, label %11

9:                                                ; preds = %4
  %10 = sub nuw nsw i32 50, %7
  tail call void @SDL_Delay(i32 noundef %10) #5
  br label %11

11:                                               ; preds = %9, %4
  %12 = load ptr, ptr @Renderer, align 8, !tbaa !5
  tail call void @SDL_RenderPresent(ptr noundef %12) #5
  ret void
}

declare void @SDL_PumpEvents() local_unnamed_addr #1

declare i32 @SDL_HasEvent(i32 noundef) local_unnamed_addr #1

; Function Attrs: cold noreturn nounwind
declare void @__assert_fail(ptr noundef, ptr noundef, i32 noundef, ptr noundef) local_unnamed_addr #4

declare i32 @SDL_GetTicks() local_unnamed_addr #1

declare void @SDL_Delay(i32 noundef) local_unnamed_addr #1

declare void @SDL_RenderPresent(ptr noundef) local_unnamed_addr #1

declare i32 @SDL_RenderDrawPoint(ptr noundef, i32 noundef, i32 noundef) local_unnamed_addr #1

; Function Attrs: nounwind sspstrong uwtable
define dso_local i32 @simRand() local_unnamed_addr #0 {
  %1 = tail call i32 @rand() #5
  ret i32 %1
}

; Function Attrs: nounwind
declare i32 @rand() local_unnamed_addr #2

attributes #0 = { nounwind sspstrong uwtable "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #1 = { "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #2 = { nounwind "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #3 = { mustprogress nocallback nofree nosync nounwind willreturn memory(argmem: readwrite) }
attributes #4 = { cold noreturn nounwind "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #5 = { nounwind }
attributes #6 = { cold noreturn nounwind }

!llvm.module.flags = !{!0, !1, !2, !3}
!llvm.ident = !{!4}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{i32 8, !"PIC Level", i32 2}
!2 = !{i32 7, !"PIE Level", i32 2}
!3 = !{i32 7, !"uwtable", i32 2}
!4 = !{!"clang version 20.1.8"}
!5 = !{!6, !6, i64 0}
!6 = !{!"p1 _ZTS12SDL_Renderer", !7, i64 0}
!7 = !{!"any pointer", !8, i64 0}
!8 = !{!"omnipotent char", !9, i64 0}
!9 = !{!"Simple C/C++ TBAA"}
!10 = !{!11, !11, i64 0}
!11 = !{!"int", !8, i64 0}
!12 = distinct !{!12, !13}
!13 = !{!"llvm.loop.unroll.disable"}
!14 = !{!15, !15, i64 0}
!15 = !{!"p1 _ZTS10SDL_Window", !7, i64 0}
