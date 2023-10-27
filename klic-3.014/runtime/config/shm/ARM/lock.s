	.arch armv7-a
	.eabi_attribute 28, 1
	.eabi_attribute 20, 1
	.eabi_attribute 21, 1
	.eabi_attribute 23, 3
	.eabi_attribute 24, 1
	.eabi_attribute 25, 1
	.eabi_attribute 26, 2
	.eabi_attribute 30, 1
	.eabi_attribute 34, 1
	.eabi_attribute 18, 4
	.file	"spinlock.c"
	.text
	.align	1
	.global	s_lock
	.arch armv7-a
	.syntax unified
	.thumb
	.thumb_func
	.fpu vfpv3-d16
	.type	s_lock, %function
s_lock:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	@ link register save eliminated.
	movs	r2, #1
.L4:
	ldrexb	r3, [r0]
	strexb	r1, r2, [r0]
	cmp	r1, #0
	bne	.L4
	dmb	ish
	uxtb	r3, r3
	cmp	r3, #0
	bne	.L4
	bx	lr
	.size	s_lock, .-s_lock
	.align	1
	.global	s_unlock
	.syntax unified
	.thumb
	.thumb_func
	.fpu vfpv3-d16
	.type	s_unlock, %function
s_unlock:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	@ link register save eliminated.
	dmb	ish
	movs	r3, #0
	strb	r3, [r0]
	bx	lr
	.size	s_unlock, .-s_unlock
	.ident	"GCC: (Ubuntu 9.3.0-17ubuntu1~20.04) 9.3.0"
	.section	.note.GNU-stack,"",%progbits
