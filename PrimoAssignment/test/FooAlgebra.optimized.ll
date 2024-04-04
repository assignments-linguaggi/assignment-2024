; ModuleID = 'TEST/FooAlgebra.optimized.bc'
source_filename = "TEST/Foo.ll"

define dso_local i32 @foo(i32 noundef %0, i32 noundef %1) {
  %3 = shl i32 %0, 1
  %4 = sdiv i32 %3, 4
  %5 = mul nsw i32 %1, %4
  ret i32 %5
}
