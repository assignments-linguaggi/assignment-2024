; int MultiInstr(int e, int a, int b) {
;   a = a * 15;
;   b = a / 8;
;   b = e << 1;
;   int d = b / 4;
;   return c * d;
; }
; %3 = a / %1 = b / %4 = c

define dso_local i32 @foo2(i32 noundef %0, i32 noundef %1) #0 {
  %3 = mul nsw i32 %3, 15
  %1 = sdiv nsw i32 %3, 8
  %5 = shl i32 %0, 1
  %6 = sdiv i32 %5, 4
  %7 = mul nsw i32 %4, %6
  ret i32 %7
}
