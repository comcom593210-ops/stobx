; ModuleID = 'StobxModule'
source_filename = "StobxModule"

@intFormat = private unnamed_addr constant [4 x i8] c"%d\0A\00", align 1
@stringFormat = private unnamed_addr constant [4 x i8] c"%s\0A\00", align 1
@trueString = private unnamed_addr constant [5 x i8] c"true\00", align 1
@falseString = private unnamed_addr constant [6 x i8] c"false\00", align 1
@0 = private unnamed_addr constant [2 x i8] c"j\00", align 1

define i32 @main() {
entry:
  %0 = call i32 (ptr, ...) @printf(ptr @stringFormat, ptr @0)
  ret i32 0
}

declare i32 @printf(ptr, ...)
