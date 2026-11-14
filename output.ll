; ModuleID = 'StobxModule'
source_filename = "StobxModule"

@intFormat = private unnamed_addr constant [4 x i8] c"%d\0A\00", align 1
@stringFormat = private unnamed_addr constant [4 x i8] c"%s\0A\00", align 1
@trueString = private unnamed_addr constant [5 x i8] c"true\00", align 1
@falseString = private unnamed_addr constant [6 x i8] c"false\00", align 1

define i32 @multiply(i32 %a, i32 %b) {
entry:
  %a1 = alloca i32, align 4
  store i32 %a, ptr %a1, align 4
  %b2 = alloca i32, align 4
  store i32 %b, ptr %b2, align 4
  %a3 = load i32, ptr %a1, align 4
  %b4 = load i32, ptr %b2, align 4
  %multmp = mul i32 %a3, %b4
  ret i32 %multmp
}

define i32 @main() {
entry:
  %calltmp = call i32 @multiply(i32 5, i32 68)
  %resu = alloca i32, align 4
  store i32 %calltmp, ptr %resu, align 4
  %resu1 = load i32, ptr %resu, align 4
  %0 = call i32 (ptr, ...) @printf(ptr @intFormat, i32 %resu1)
  ret i32 0
}

declare i32 @printf(ptr, ...)
