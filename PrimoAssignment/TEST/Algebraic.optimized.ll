;Algebraic Identity # Algebraic.optimized.ll

;int Algebraic(int x)
;{
;   x = x + 0;
;   x = x * 1;
;   return x;
;}

;%0 = x

define dso_local noundef i32 @_Z9Algebraici(i32 noundef %0) {
  %2 = alloca i32, align 4
  store i32 %0, ptr %2, align 4
  %3 = load i32, ptr %2, align 4
  store i32 %3, ptr %2, align 4
  %4 = load i32, ptr %2, align 4
  store i32 %4, ptr %2, align 4
  %5 = load i32, ptr %2, align 4
  ret i32 %5
}
