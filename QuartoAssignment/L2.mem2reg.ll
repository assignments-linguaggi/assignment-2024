; ModuleID = 'L2.mem2reg.bc'
source_filename = "L2.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

@.str = private unnamed_addr constant [25 x i8] c"%d,%d,%d,%d,%d,%d,%d,%d\0A\00", align 1

; Function Attrs: noinline nounwind uwtable
define dso_local void @foo(i32 noundef %0, i32 noundef %1) #0 {
  br label %3

3:                                                ; preds = %5, %2
  %.013 = phi i32 [ 0, %2 ], [ %14, %5 ]
  %.011 = phi i32 [ undef, %2 ], [ %8, %5 ]
  %.09 = phi i32 [ 0, %2 ], [ %12, %5 ]
  %.07 = phi i32 [ 0, %2 ], [ %11, %5 ]
  %.05 = phi i32 [ undef, %2 ], [ %10, %5 ]
  %.03 = phi i32 [ 9, %2 ], [ %9, %5 ]
  %.01 = phi i32 [ %1, %2 ], [ %6, %5 ]
  %.0 = phi i32 [ 0, %2 ], [ %13, %5 ]
  %4 = icmp slt i32 %.01, 5
  br i1 %4, label %5, label %15

5:                                                ; preds = %3
  %6 = add nsw i32 %.01, 1
  %7 = add nsw i32 %0, 3
  %8 = add nsw i32 %0, 7
  %9 = add nsw i32 %.03, 2
  %10 = add nsw i32 %0, 3
  %11 = add nsw i32 %7, 7
  %12 = add nsw i32 %10, 2
  %13 = add nsw i32 %0, 7
  %14 = add nsw i32 %8, 5
  br label %3, !llvm.loop !6

15:                                               ; preds = %3
  br label %16

16:                                               ; preds = %18, %15
  %.114 = phi i32 [ %.013, %15 ], [ %27, %18 ]
  %.112 = phi i32 [ %.011, %15 ], [ %21, %18 ]
  %.110 = phi i32 [ %.09, %15 ], [ %25, %18 ]
  %.18 = phi i32 [ %.07, %15 ], [ %24, %18 ]
  %.16 = phi i32 [ %.05, %15 ], [ %23, %18 ]
  %.14 = phi i32 [ %.03, %15 ], [ %22, %18 ]
  %.12 = phi i32 [ %.01, %15 ], [ %19, %18 ]
  %.1 = phi i32 [ %.0, %15 ], [ %26, %18 ]
  %17 = icmp slt i32 %.12, 10
  br i1 %17, label %18, label %28

18:                                               ; preds = %16
  %19 = add nsw i32 %.12, 1
  %20 = add nsw i32 %0, 3
  %21 = add nsw i32 %0, 7
  %22 = sub nsw i32 %.14, 1
  %23 = add nsw i32 %0, 4
  %24 = add nsw i32 %20, 7
  %25 = add nsw i32 %23, 2
  %26 = add nsw i32 %0, 7
  %27 = add nsw i32 %21, 5
  br label %16, !llvm.loop !8

28:                                               ; preds = %16
  %29 = call i32 (ptr, ...) @printf(ptr noundef @.str, i32 noundef %.14, i32 noundef %.16, i32 noundef %.18, i32 noundef %.110, i32 noundef %.112, i32 noundef %.114, i32 noundef %.1, i32 noundef %.12)
  ret void
}

declare i32 @printf(ptr noundef, ...) #1

; Function Attrs: noinline nounwind uwtable
define dso_local i32 @main() #0 {
  call void @foo(i32 noundef 0, i32 noundef 4)
  call void @foo(i32 noundef 0, i32 noundef 12)
  ret i32 0
}

attributes #0 = { noinline nounwind uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #1 = { "frame-pointer"="all" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }

!llvm.module.flags = !{!0, !1, !2, !3, !4}
!llvm.ident = !{!5}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{i32 8, !"PIC Level", i32 2}
!2 = !{i32 7, !"PIE Level", i32 2}
!3 = !{i32 7, !"uwtable", i32 2}
!4 = !{i32 7, !"frame-pointer", i32 2}
!5 = !{!"clang version 17.0.6 (https://github.com/rxgezz/first-assignment-compilatori.git 982f88a7fc9126bf64f3b9b1a9da60ebc8963e2b)"}
!6 = distinct !{!6, !7}
!7 = !{!"llvm.loop.mustprogress"}
!8 = distinct !{!8, !7}
