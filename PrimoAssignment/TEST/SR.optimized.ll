;Strength Reduction # SR.optimized.ll

;int SR(int x)
;{
;    x = x * 15;
;    int y = x / 8;
;    return y;
;}

;%0 = x

define dso_local noundef i32 @_Z2SRi(i32 noundef %0) local_unnamed_addr {
  %shl = shl i32 %0, 4
  %sub = sub i32 %shl, %0
  %shr = ashr i32 %sub, 3
  ret i32 %shr
}
