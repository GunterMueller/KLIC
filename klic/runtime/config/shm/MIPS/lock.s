	.set	noreorder
	.set	noat
	#	/usr/lib64/cmplrs/be
	#ident	"$Source: /home/igarashi/kl1/cvsroot/klic/runtime/config/shm/MIPS/lock.s,v $ $Revision: 1.1.1.1 $"
	#ident	"$Source: /home/igarashi/kl1/cvsroot/klic/runtime/config/shm/MIPS/lock.s,v $ $Revision: 1.1.1.1 $"
	#Options: -G8 -m1 -TARG:abi=64:isa=mips4 -O3 -s
	.file	1	"/work/yosinari/klic-1.700c5/runtime/config.comm/shm/irix/test/lock_s.c"

	.section .text, 1, 0x00000006, 4, 16
.text:
	.section .text

	# Program Unit: s_lock
	.ent	s_lock
	.globl	s_lock
s_lock: 	 # 0x0
	.frame	$sp, 0, $31
.BB1.s_lock: 	 # 0x0
	.loc	1 4
 #   1  
 #   2  void s_lock(int*lockwdp)
 #   3  {
 #   4    *lockwdp |= 1;
 #	lw $1,0($4)                    # [0]  
 #	ori $1,$1,1                    # [1]  
 loop:
	ll $2,($4)
	ori $3,$2,1
	beq $3,$2,loop
	nop

	sc $3,($4)
	beq $3,0,loop
	nop

	.loc	1 5
 #   5  }
	j $31                          # [2]  
 #	sw $1,0($4)                    # [2]  
	.end	s_lock
	.section .text

	# Program Unit: s_unlock
	.ent	s_unlock
	.globl	s_unlock
s_unlock: 	 # 0x10
	.frame	$sp, 0, $31
.BB5.s_unlock: 	 # 0x10
	.loc	1 9
 #   6  
 #   7  void s_unlock(int*lockwdp)
 #   8  {
 #   9    *lockwdp &= 0xfffffffe;
	lw $1,0($4)                    # [0]  
	addiu $2,$0,-2                 # [0]  
	and $1,$1,$2                   # [1]  
	.loc	1 10
 #  10  }
	j $31                          # [2]  
	sw $1,0($4)                    # [2]  
	.end	s_unlock
	.section .text
	.align 4
