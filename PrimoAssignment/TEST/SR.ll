;Strength Reduction # SR.ll

;int SR(int x)
;{
;    x = x * 15;
;    int y = x / 8;
;    return y;
;}

;%0 = x

define dso_local noundef i32 @_Z2SRi(i32 noundef %0) local_unnamed_addr #0 {
  %2 = mul nsw i32 %0, 15 ; To be otpimized
  %3 = sdiv i32 %2, 8 ; To be optimized
  ret i32 %3
}