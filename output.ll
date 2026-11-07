; ModuleID = 'StobxModule'
source_filename = "StobxModule"

@intFormat = private unnamed_addr constant [4 x i8] c"%d\0A\00", align 1
@stringFormat = private unnamed_addr constant [4 x i8] c"%s\0A\00", align 1
@trueString = private unnamed_addr constant [5 x i8] c"true\00", align 1
@falseString = private unnamed_addr constant [6 x i8] c"false\00", align 1

define i32 @main() {
entry:
  %a = alloca i32, align 4
  store i32 1, ptr %a, align 4
  br label %while.condition

while.condition:                                  ; preds = %while.body, %entry
  %a1 = load i32, ptr %a, align 4
  %cmptmp = icmp slt i32 %a1, 5
  br i1 %cmptmp, label %while.body, label %while.end

while.body:                                       ; preds = %while.condition
  %a2 = load i32, ptr %a, align 4
  %0 = call i32 (ptr, ...) @printf(ptr @intFormat, i32 %a2)
  %a3 = load i32, ptr %a, align 4
  %addtmp = add i32 %a3, 1
  store i32 %addtmp, ptr %a, align 4
  br label %while.condition

while.end:                                        ; preds = %while.condition
  ret i32 0
}

declare i32 @printf(ptr, ...)
