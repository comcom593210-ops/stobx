; ModuleID = 'StobxModule'
source_filename = "StobxModule"

@intFormat = private unnamed_addr constant [4 x i8] c"%d\0A\00", align 1
@stringFormat = private unnamed_addr constant [4 x i8] c"%s\0A\00", align 1
@trueString = private unnamed_addr constant [5 x i8] c"true\00", align 1
@falseString = private unnamed_addr constant [6 x i8] c"false\00", align 1

define i32 @main() {
entry:
  %a = alloca i32, align 4
  store i32 640, ptr %a, align 4
  %b = alloca i32, align 4
  store i32 6, ptr %b, align 4
  %a1 = load i32, ptr %a, align 4
  %b2 = load i32, ptr %b, align 4
  %lshrtmp = lshr i32 %a1, %b2
  %0 = call i32 (ptr, ...) @printf(ptr @intFormat, i32 %lshrtmp)
  ret i32 0
}

declare i32 @printf(ptr, ...)
