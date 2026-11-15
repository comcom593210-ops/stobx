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
  %cmptmp = icmp slt i32 %i1, 5
  br i1 %cmptmp, label %for.body, label %for.end

for.body:                                         ; preds = %for.condition
  %i2 = load i32, ptr %i, align 4
  %0 = call i32 (ptr, ...) @printf(ptr @intFormat, i32 %i2)
  br label %for.update

for.update:                                       ; preds = %for.body
  %i3 = load i32, ptr %i, align 4
  %addtmp = add i32 %i3, 1
  store i32 %addtmp, ptr %i, align 4
  br label %for.condition

for.end:                                          ; preds = %for.condition
  %j = alloca i32, align 4
  store i32 5, ptr %j, align 4
  br label %for.condition4

for.condition4:                                   ; preds = %for.update6, %for.end
  %j8 = load i32, ptr %j, align 4
  %cmptmp9 = icmp slt i32 %j8, 9
  br i1 %cmptmp9, label %for.body5, label %for.end7

for.body5:                                        ; preds = %for.condition4
  %j10 = load i32, ptr %j, align 4
  %1 = call i32 (ptr, ...) @printf(ptr @intFormat, i32 %j10)
  br label %for.update6

for.update6:                                      ; preds = %for.body5
  %j11 = load i32, ptr %j, align 4
  %addtmp12 = add i32 %j11, 1
  store i32 %addtmp12, ptr %j, align 4
  br label %for.condition4

for.end7:                                         ; preds = %for.condition4
  %k = alloca i32, align 4
  store i32 10, ptr %k, align 4
  br label %for.condition13

for.condition13:                                  ; preds = %for.update15, %for.end7
  %k17 = load i32, ptr %k, align 4
  %cmptmp18 = icmp slt i32 %k17, 5
  br i1 %cmptmp18, label %for.body14, label %for.end16

for.body14:                                       ; preds = %for.condition13
  %k19 = load i32, ptr %k, align 4
  %2 = call i32 (ptr, ...) @printf(ptr @intFormat, i32 %k19)
  br label %for.update15

for.update15:                                     ; preds = %for.body14
  %k20 = load i32, ptr %k, align 4
  %addtmp21 = add i32 %k20, 1
  store i32 %addtmp21, ptr %k, align 4
  br label %for.condition13

for.end16:                                        ; preds = %for.condition13
  %m = alloca i32, align 4
  store i32 1, ptr %m, align 4
  br label %for.condition22

for.condition22:                                  ; preds = %for.update24, %for.end16
  %m26 = load i32, ptr %m, align 4
  %cmptmp27 = icmp slt i32 %m26, 4
  br i1 %cmptmp27, label %for.body23, label %for.end25

for.body23:                                       ; preds = %for.condition22
  %m28 = load i32, ptr %m, align 4
  %3 = call i32 (ptr, ...) @printf(ptr @intFormat, i32 %m28)
  %m29 = load i32, ptr %m, align 4
  %addtmp30 = add i32 %m29, 10
  %4 = call i32 (ptr, ...) @printf(ptr @intFormat, i32 %addtmp30)
  br label %for.update24

for.update24:                                     ; preds = %for.body23
  %m31 = load i32, ptr %m, align 4
  %addtmp32 = add i32 %m31, 1
  store i32 %addtmp32, ptr %m, align 4
  br label %for.condition22

for.end25:                                        ; preds = %for.condition22
  %x = alloca i32, align 4
  store i32 0, ptr %x, align 4
  br label %for.condition33

for.condition33:                                  ; preds = %for.update35, %for.end25
  %x37 = load i32, ptr %x, align 4
  %cmptmp38 = icmp slt i32 %x37, 2
  br i1 %cmptmp38, label %for.body34, label %for.end36

for.body34:                                       ; preds = %for.condition33
  %y = alloca i32, align 4
  store i32 0, ptr %y, align 4
  br label %for.condition39

for.update35:                                     ; preds = %for.end42
  %x49 = load i32, ptr %x, align 4
  %addtmp50 = add i32 %x49, 1
  store i32 %addtmp50, ptr %x, align 4
  br label %for.condition33

for.end36:                                        ; preds = %for.condition33
  ret i32 0

for.condition39:                                  ; preds = %for.update41, %for.body34
  %y43 = load i32, ptr %y, align 4
  %cmptmp44 = icmp slt i32 %y43, 2
  br i1 %cmptmp44, label %for.body40, label %for.end42

for.body40:                                       ; preds = %for.condition39
  %x45 = load i32, ptr %x, align 4
  %5 = call i32 (ptr, ...) @printf(ptr @intFormat, i32 %x45)
  %y46 = load i32, ptr %y, align 4
  %6 = call i32 (ptr, ...) @printf(ptr @intFormat, i32 %y46)
  br label %for.update41

for.update41:                                     ; preds = %for.body40
  %y47 = load i32, ptr %y, align 4
  %addtmp48 = add i32 %y47, 1
  store i32 %addtmp48, ptr %y, align 4
  br label %for.condition39

for.end42:                                        ; preds = %for.condition39
  br label %for.update35
}

declare i32 @printf(ptr, ...)
