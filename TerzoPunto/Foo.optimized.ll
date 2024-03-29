; ModuleID = 'TEST/FooMulti.optimized.bc'
source_filename = "TEST/Foo2.ll"

define dso_local i32 @foo2(i32 noundef %0, i32 noundef %1) {
  %3 = add nsw i32 %1, 1
  %4 = shl i32 %0, 1
  %5 = sdiv i32 %4, 4
  %6 = mul nsw i32 %1, %5
  ret i32 %6
}
