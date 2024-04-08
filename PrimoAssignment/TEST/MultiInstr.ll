;Multi-Instruction Optimization # MultiInstr.ll

;int MultiInstr(int b)
;{
;    int a = b + 1;
;    int c = a - 1;
;    return c;
;}

;%0 = b
;%6 = a
;%8 = c

define dso_local noundef i32 @_Z10MultiInstri(i32 noundef %0) #0 {
  %2 = alloca i32, align 4
  %3 = alloca i32, align 4
  %4 = alloca i32, align 4
  store i32 %0, ptr %2, align 4
  %5 = load i32, ptr %2, align 4
  %6 = add nsw i32 %5, 1
  store i32 %6, ptr %3, align 4
  %7 = load i32, ptr %3, align 4
  %8 = sub nsw i32 %7, 1  ; To be optimized
  store i32 %8, ptr %4, align 4
  %9 = load i32, ptr %4, align 4
  ret i32 %9
}