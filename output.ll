; ModuleID = 'StobxModule'
source_filename = "StobxModule"

@intFormat = private unnamed_addr constant [4 x i8] c"%d\0A\00", align 1
@stringFormat = private unnamed_addr constant [4 x i8] c"%s\0A\00", align 1
@trueString = private unnamed_addr constant [5 x i8] c"true\00", align 1
@falseString = private unnamed_addr constant [6 x i8] c"false\00", align 1

define i32 @main() {
entry:
  %i = alloca i32, align 4
  store i32 0, ptr %i, align 4
  br label %for.condition

for.condition:                                    ; preds = %for.update, %entry
  %i1 = load i32, ptr %i, align 4
  %cmptmp = icmp slt i32 %i1, 3
  br i1 %cmptmp, label %for.body, label %for.end

for.body:                                         ; preds = %for.condition
  %j = alloca i32, align 4
  store i32 0, ptr %j, align 4
  br label %for.condition2

for.update:                                       ; preds = %for.end5
  %i13 = load i32, ptr %i, align 4
  %addtmp14 = add i32 %i13, 1
  store i32 %addtmp14, ptr %i, align 4
  br label %for.condition

for.end:                                          ; preds = %for.condition
  ret i32 0

for.condition2:                                   ; preds = %for.update4, %for.body
  %j6 = load i32, ptr %j, align 4
  %cmptmp7 = icmp slt i32 %j6, 5
  br i1 %cmptmp7, label %for.body3, label %for.end5

for.body3:                                        ; preds = %for.condition2
  %j8 = load i32, ptr %j, align 4
  %cmptmp9 = icmp eq i32 %j8, 2
  br i1 %cmptmp9, label %if.then, label %if.else

for.update4:                                      ; preds = %if.end
  %j11 = load i32, ptr %j, align 4
  %addtmp = add i32 %j11, 1
  store i32 %addtmp, ptr %j, align 4
  br label %for.condition2

for.end5:                                         ; preds = %if.then, %for.condition2
  %i12 = load i32, ptr %i, align 4
  %0 = call i32 (ptr, ...) @printf(ptr @intFormat, i32 %i12)
  br label %for.update

if.then:                                          ; preds = %for.body3
  br label %for.end5

if.else:                                          ; preds = %for.body3
  br label %if.end

if.end:                                           ; preds = %if.else
  %j10 = load i32, ptr %j, align 4
  %1 = call i32 (ptr, ...) @printf(ptr @intFormat, i32 %j10)
  br label %for.update4
}

declare i32 @printf(ptr, ...)
