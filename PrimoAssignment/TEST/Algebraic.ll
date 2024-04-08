;Algebraic Identity # Algebraic.ll

;int Algebraic(int x)
;{
;   x = x + 0;
;   x = x * 1;
;   return x;
;}

;%0 = x

define dso_local noundef i32 @_Z9Algebraici(i32 noundef %0) #0 {
  %2 = alloca i32, align 4
  store i32 %0, ptr %2, align 4
  %3 = load i32, ptr %2, align 4
  %4 = add nsw i32 %3, 0  ; To be optimized
  store i32 %4, ptr %2, align 4
  %5 = load i32, ptr %2, align 4
  %6 = mul nsw i32 %5, 1  ; To be optimized
  store i32 %6, ptr %2, align 4
  %7 = load i32, ptr %2, align 4
  ret i32 %7
}