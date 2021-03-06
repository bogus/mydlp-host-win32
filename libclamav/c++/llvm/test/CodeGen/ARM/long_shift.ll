; RUN: llc < %s -march=arm | FileCheck %s

define i64 @f0(i64 %A, i64 %B) {
; CHECK: f0
; CHECK:      movs    r3, r3, lsr #1
; CHECK-NEXT: mov     r2, r2, rrx
; CHECK-NEXT: subs    r0, r0, r2
; CHECK-NEXT: sbc     r1, r1, r3
	%tmp = bitcast i64 %A to i64
	%tmp2 = lshr i64 %B, 1
	%tmp3 = sub i64 %tmp, %tmp2
	ret i64 %tmp3
}

define i32 @f1(i64 %x, i64 %y) {
; CHECK: f1
; CHECK: mov r0, r0, lsl r2
	%a = shl i64 %x, %y
	%b = trunc i64 %a to i32
	ret i32 %b
}

define i32 @f2(i64 %x, i64 %y) {
; CHECK: f2
; CHECK:      mov     r0, r0, lsr r2
; CHECK-NEXT: rsb     r12, r2, #32
; CHECK-NEXT: sub     r2, r2, #32
; CHECK-NEXT: cmp     r2, #0
; CHECK-NEXT: orr     r0, r0, r1, lsl r12
; CHECK-NEXT: movge   r0, r1, asr r2
	%a = ashr i64 %x, %y
	%b = trunc i64 %a to i32
	ret i32 %b
}

define i32 @f3(i64 %x, i64 %y) {
; CHECK: f3
; CHECK:      mov     r0, r0, lsr r2
; CHECK-NEXT: rsb     r12, r2, #32
; CHECK-NEXT: sub     r2, r2, #32
; CHECK-NEXT: cmp     r2, #0
; CHECK-NEXT: orr     r0, r0, r1, lsl r12
; CHECK-NEXT: movge   r0, r1, lsr r2
	%a = lshr i64 %x, %y
	%b = trunc i64 %a to i32
	ret i32 %b
}
